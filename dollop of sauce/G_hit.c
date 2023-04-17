/*
	G_HIT.C


	hitting stuff routines

	should be called after collision occurs between objects or between
	objects and the landscape

	TODO: see some of the functions below
*/
#include "G_Hit.h"
#include "G_Objects.h"
#include "G_AiTypes.h"
#include "G_Ai.h"
#include "G_PlayerAnim.h"
#include "G_Init.h"
#include "G_Stat.h"
#include "Random.h"
#include "G_Event.h"
#include "E_Sound.h"
#include "E_Loaders.h"
#include "E_Strings.h"
#include "E_Particles.h"
#include "Globals.h"
#include "Grade.h"
#include "N_Chat.h"
#include "N_main.h"

/*
	DoPoints

	gives player points for picking up something

	TODO: show score flying off
*/
void DoPoints(int nump,int num)
{

	//also in release, otherwise this can CRASH !!!!
	if ((nump<PLAYSLOT) || (nump>PLAYSLOT+MAXPLAYERS) || (num>=MAXOBJECTS))
	{
		return;
	}

	if ((gameobj[num].points>0) && 
		(MultiPlayerType!=gameBATTLE) &&
		(MultiPlayerType!=gameRACE) &&
		(MultiPlayerType!=gameCAPTURE) &&
		(MultiPlayerType!=gameTREASURE))
	{		
		player[nump-PLAYSLOT].score+=gameobj[num].points;

		AddParticleScore(gameobj[num].xpos,gameobj[num].ypos,gameobj[num].points);

		gameobj[num].points=0; //dont give twice for whatever reason
	}
} //DoPoints()



/*
	plays a sample when picking up a pickup.
	does frequency shifting for each consequent pickup
*/
void PickupSample(int x,int y)
{
	static int Rate=16025;
	static int Rdir=1;
	static int LastGameCounter=0;
	int delta;


	delta=gamecounter-LastGameCounter;

	if (delta>AISPEED*2)
	{
		Rate=16025;
		Rdir=1;
	}

	LastGameCounter=gamecounter;

	if (Rdir==1)
	{
		Rate=(Rate*69433)/65536;
		if (Rate>28050) 
		{
			Rate=28050;
			Rdir=0;
		}
	}
	else
	{
		Rate=(Rate*58386)/65536;
		if (Rate<16025)
		{
			Rdir=1;
			Rate=16025;
		}
	}
	PlaySample(x,y,sCOMMON_PICKUP1,63,Rate);
} //PickupSample()




/*
	adds something extra when you kill something. 

	adds fastfire, energy or diamonds
*/
void AddExtra(int nump,int num,int freq)
{
	int rnd,res;


	if (MultiPlayerType>gameCOOPERATIVE)		//no gems here......
		return;

	ASSERT(num<MAXOBJECTS);
	if (gameobj[num].creator<LEVELSLOT)	//generated event
	{
		return;	//dont give stuff!!!
	}


	if ((nump<PLAYSLOT) || (nump>=PLAYSLOT+MAXPLAYERS))
	{
		return;
	}


	player[nump-PLAYSLOT].extracounter++;	//frequency counter

	if ((player[nump-PLAYSLOT].extracounter&7)==0)
	{
		res=AddObject(gameobj[num].xpos,gameobj[num].ypos-RandFac(32767)*32,aGUNFASTFIRE,nump);
		if ((res>0) && (player[nump-PLAYSLOT].character==mSPAZ))
			gameobj[res].curanim=AnimBase[mPICKUPS]+mPICKUPS_FASTSPAZ; //change to green gun for SPAZ
	}
	else
	{
		rnd=RandFac(16383);
		if ((rnd%freq)==0)		//MOD! argh
		{
			rnd=(rnd>>8);
			switch (rnd&3) {
			case 0:
				AddObject(gameobj[num].xpos,gameobj[num].ypos-RandFac(32767)*32,aENERGYUP,nump);
				break;
			case 1:
				AddObject(gameobj[num].xpos,gameobj[num].ypos-RandFac(32767)*32,aREDGEM,nump);
				break;
			case 2:
				AddObject(gameobj[num].xpos,gameobj[num].ypos-RandFac(32767)*32,aGREENGEM,nump);
				break;
			default:
			case 3:
				AddObject(gameobj[num].xpos,gameobj[num].ypos-RandFac(32767)*32,aBLUEGEM,nump);
				break;
			}	//switch
		}
	}	//other
} //AddExtra()






/*
	throw player items around when one of the players dies!

	used in battle mode to throw around player ammo
*/

static int ScatterAmmo[8]={
	aGUN2AMMO3,aGUN3AMMO3,aGUN4AMMO3,aGUN5AMMO3,
	aGUN6AMMO3,aGUN7AMMO3,aGUN8AMMO3,aGUN9AMMO3
};
void ScatterPlayerItems(int nump)
{
	Tplayer	*play;
	Omonster *obj;
	int t,cobj,calc;


	play=&player[nump];


	for (t=0;t<8;t++)
	{
		calc=play->ammo[t+1];
		while (calc>3)
		{
			cobj=AddObject(play->xpos+Rand2Fac(32767)*16,play->ypos-RandFac(32767)*8,ScatterAmmo[t],0);
			if (cobj>0)
			{
				obj=(Omonster *)(&gameobj[cobj]);
				obj->objtype=oWAITPICKUP;
				obj->var3=AISPEED/4;	//dont let this player pick it up
				obj->xspeed=Rand2Fac(32767)*16;
				obj->yspeed=Rand2Fac(32767)*8;
				if (obj->xspeed<0)
					obj->direction=-1;
				calc-=3;
			}
			else
				break; //no more objects
		} //while
	} //for all ammo
} //ScatterPlayerItems()





//static int foreverdiamonds[4]=
//	{aREDGEM,aGREENGEM,aBLUEGEM,aPURPLEGEM};

static int JAZZHURTSOUNDS[8]=
{sJAZZSOUNDS_HEY1,sJAZZSOUNDS_HEY2,sJAZZSOUNDS_HEY3,sJAZZSOUNDS_HEY4,
sJAZZSOUNDS_JAZZV1,sJAZZSOUNDS_JAZZV2,sJAZZSOUNDS_JAZZV3,sJAZZSOUNDS_JAZZV4};

/*
	hitplayer

	hit a player.
	in this function all decisions are made if the player is hurt, or
	if the bird/shield dissappears first
*/
int HitPlayer(int nump,int numobj)
{
	Tplayer *play;
	Obullets *bull;
	Word8 objfreeze;
	int t,res;
	int random;
	int dohurt;
	int abletohit;
	Word8 data[128];
	TactPACKETS *action;

	if ((numobj>0) && (numobj<PLAYSLOT))
		objfreeze=((Omonster *)&gameobj[numobj])->freeze;
	else
		objfreeze=0;

	BOUNDS(nump,0,MAXPLAYERS);
	play=&player[nump];

	abletohit=dohurt=0;

	if (MultiPlayerConnection==gameLOCAL)
	{
		if ((play->hit<=0) && 
			(play->downattack>=DOWNATTACKWAIT) && 
			(play->invincibility==0) && 
			(play->rush==0) &&
			(level.finish==0) &&
			(play->flicker==0) &&
			(objfreeze==0))
		{
			abletohit=1;
			dohurt=1;
		}
	}
	else
	{	//netgame

		if (LocalComputerIsServer)
		{

			if (MultiPlayerType==gameCAPTURE)
			{
				if (numobj<=0)
					res=nump;	//kill yourself
				else
				if (numobj<PLAYSLOT)
					res=gameobj[numobj].creator-PLAYSLOT;
				else
					res=numobj-PLAYSLOT;

				if ((res!=nump) && (player[res].Team==player[nump].Team))
				{
					//go away
					return(0);	//dont able to hit
				}
				dohurt=1;	//kill yourself... possible
			}
			else
				dohurt=1;
		}

		if ((play->hit<=0) && 
			(play->flicker==0))
		{
			abletohit=1;
		}
	}	//net


	if (abletohit) {
		if (play->bird>0) {
			//gameobj[play->bird-1].state=sHIT;
			play->invincibility=-AISPEED*3;
			play->flicker=-AISPEED*3;
			play->bird=0;
			play->hit=HITTIME;	//INVINCIBLE TIME! SHOULD FLICKER...
			play->idletime=0;
		} else if (play->shield>0) {
			play->invincibility=-AISPEED*3;
			play->flicker=-AISPEED*3;
			play->idletime=0;
			play->shield-=5*AISPEED;


			if (play->shieldtype==3) { //electric shield
				for (t=0;t<10;t++) {
					res=AddObject(play->xpos,play->ypos,aSHARD,nump+PLAYSLOT);
					if (res>0) {
						gameobj[res].curanim=AnimBase[mAMMO]+mAMMO_SPARK1;
						gameobj[res].light=1;
						gameobj[res].lighttype=2;
					} else {
						break;
					}
				}
			} else if (play->shieldtype==1) {
				for (t=0;t<10;t++) {
					res=AddObject(play->xpos,play->ypos,aBOUNCEONCE,nump+PLAYSLOT);
					if (res>0) {
						gameobj[res].curanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
						gameobj[res].light=1;
						gameobj[res].lighttype=2;
					} else {
						break;
					}
				}
			}

			if (LocalComputerIsServer && (MultiPlayerConnection>gameLOCAL) && (play->localplayer==0)) {
				//only in network where this computer is the server, and only to the client affected
				action=Server_AddAction(actPLAYERHITSHIELD,play->Client);
				action->PLAYERHITSHIELD.nump=nump;
			} else {
				if (play->shield<=0) {
					PlaySample(play->xpos,play->ypos,sCOMMON_SHLDOF3,0,0);
					play->shield=-1; //show dissappearing shield
					play->invincibility=-AISPEED*3;
					play->flicker=-AISPEED*3;
				}
			}

			if (play->character==mSPAZ) {
				PlaySample(play->xpos,play->ypos,sSPAZSOUNDS_OOOH,0,0);
			} else if (play->character==mJAZZ) {
				PlaySample(play->xpos,play->ypos,JAZZHURTSOUNDS[RandFac(7)],0,0);
			}
		} else {
			if ((MultiPlayerType==gameTREASURE) && 
				((numobj>=PLAYSLOT) ||
				 (numobj>0) && (numobj<LEVELSLOT) && (gameobj[numobj].objtype==oPLAYERBULLET))) {	
				if (numobj>=PLAYSLOT) {
					t=10;
				} else {
					bull=(Obullets *)&gameobj[numobj];

					if ((bull->var4<=3) || (bull->var4==6))
						t=3;
					else if (bull->load==aPLAYERLASER)
						t=25;
					else 			
						t=10;
				}


				if (t>play->gem[1])	//just red gems
					t=play->gem[1];

				if (t) {
					int xspeed,yspeed;

					if (numobj>=PLAYSLOT) {
						xspeed=player[numobj-PLAYSLOT].xspeed/(2*65536);
						yspeed=player[numobj-PLAYSLOT].yspeed/(2*65536);
					} else if ((numobj>0) && (numobj<NumObjects)) {
						xspeed=gameobj[numobj].xspeed/(2*65536);
						yspeed=gameobj[numobj].yspeed/(2*65536);
					} else {
						xspeed=0; 
						yspeed=0;
					}

					if (xspeed<-7) xspeed=-7;
					else
					if (xspeed>7) xspeed=7;

					if (yspeed<-7) yspeed=-7;
					else
					if (yspeed>7) yspeed=7;

					if ((LocalComputerIsServer && (MultiPlayerConnection>gameLOCAL)) || 
						(MultiPlayerConnection==gameLOCAL))
					{
						int startuid=0;
						//only in local games or in network where this computer is the server

						if (MultiPlayerConnection!=gameLOCAL) {
							int Temp;
							action=(TactPACKETS *)(&data[0]);
							action->GENERIC.PacketID=actPLAYERADDTREASUREDIAMONDS;
							action->PLAYERADDTREASUREDIAMONDS.nump=nump;
							random=RandFac(2047);
							action->PLAYERADDTREASUREDIAMONDS.randomLow3=random;
							action->PLAYERADDTREASUREDIAMONDS.randomHigh=random>>3;
							startuid=play->CreateUIDCounter&1023;
							action->PLAYERADDTREASUREDIAMONDS.UIDcounterLow=startuid;
							action->PLAYERADDTREASUREDIAMONDS.UIDcounterHigh=startuid>>8;
							action->PLAYERADDTREASUREDIAMONDS.NumGems=t;
							action->PLAYERADDTREASUREDIAMONDS.xs=xspeed;
							action->PLAYERADDTREASUREDIAMONDS.ys=yspeed;
							Temp = play->xpos/65536;
							action->PLAYERADDTREASUREDIAMONDS.xposLow=Temp;
							action->PLAYERADDTREASUREDIAMONDS.xposHigh=Temp>>8;
							Temp = play->ypos/65536;
							action->PLAYERADDTREASUREDIAMONDS.yposLow=Temp;
							action->PLAYERADDTREASUREDIAMONDS.yposHigh=Temp>>8;


							Server_SendActions(action,0);

							RandInit(random);
						}	//!local!

						xspeed*=4096;
						yspeed*=2*65536;

						//when you update this code, update in n_action : Client_ProcessAction too!!!
						do {
							res=AddObject(play->xpos,play->ypos,aREDGEM,nump+PLAYSLOT);
							if (res>0)
							{
								gameobj[res].xspeed=xspeed*RandFac(63);
								gameobj[res].yspeed=yspeed-RandFac(32767)*16;

								gameobj[res].objtype=oWAITPICKUP; //wait a bit
								gameobj[res].ProcPtr=&cDiamondsAreForever;

								UID[res]=32768+(nump*1024)+(startuid&1023);
								startuid++;
								play->gem[1]--;
							}  else break;
						} while (--t);
					}	//if we are capable of deciding ;-)
				}	//if t

				play->idletime=0;
			} else {	//!treasure game
				play->idletime=0;
				if (dohurt) {
					int hurtamount=1;

					if (numobj>=PLAYSLOT) {	//special moves!
						hurtamount=2;
					} else {
						if ((numobj>0) && (numobj<NumObjects)) {
							if ((MultiPlayerType==gameBATTLE) || (MultiPlayerType==gameCAPTURE)) {
								if ((((Obullets *)(&gameobj[numobj]))->type&bullPOWER)==bullPOWER) {
									hurtamount=2;
								}
							} else if ((gameobj[numobj].objtype&oTYPES)==oENEMYBULLET) {
								hurtamount=(short)(((Obullets *)(&gameobj[numobj]))->force);
							}
						}
					}	//playslot

					play->energy-=hurtamount;
				}	//dohurt

				if (play->energy<=0)
				{
					play->energy=0;

					if ((MultiPlayerType==gameBATTLE) && (numobj>0) &&
						((MultiPlayerConnection==gameLOCAL) ||
						((MultiPlayerConnection>gameLOCAL) && LocalComputerIsServer)))
					{
						if (numobj<NumObjects)
							res=gameobj[numobj].creator-PLAYSLOT;
						else
							res=numobj-PLAYSLOT;

						if ((res>=0) && (res<MAXPLAYERS))
						{
							player[res].lapcounter++; //one kill for ya

							if (player[res].lapcounter>=level.extra)
							{
								if (player[res].finished==0)
								{
									play->energy=1; //dont do kill anim NOW
									player[res].finished=1;
									player[res].lastlapfinish=gamecounter;
									if (MultiPlayerConnection>gameLOCAL)
									{	//finished
										Server_LevelFinished(res);
									}
								}	//finsihed
							}	//made number of laps ok?
						}	//res is okay
									
												//send killshit over
						if ((MultiPlayerConnection>gameLOCAL) && LocalComputerIsServer)
						{
							action=(TactPACKETS *)(&data[0]);
							action->GENERIC.PacketID=actPLAYERKILL;
							action->PLAYERKILL.nump=nump;
							action->PLAYERKILL.nump_frags=player[nump].lapcounter;

							if ((res>=0) && (res<MAXPLAYERS))
							{
								action->PLAYERKILL.creator=res;
								action->PLAYERKILL.creator_frags=player[res].lapcounter;
							}
							else
							{
								action->PLAYERKILL.creator=255;
								action->PLAYERKILL.creator_frags=0;
							}

							Server_SendActions(action,0);
							ChatAddKillMessage(res,nump);
						}	//mutliplayer

						//ScatterPlayerItems(nump);

					}	//battle
					else
					if (MultiPlayerType==gameCAPTURE)
					{
						if (numobj<=0)
							res=nump;	//kill yourself
						else
						if (numobj<PLAYSLOT)
							res=gameobj[numobj].creator-PLAYSLOT;
						else
							res=numobj-PLAYSLOT;

						if ((res>=0) && (res<MAXPLAYERS))
						{
							//send killshit over
							if ((MultiPlayerConnection>gameLOCAL) && LocalComputerIsServer)
							{
								action=(TactPACKETS *)(&data[0]);
								action->GENERIC.PacketID=actPLAYERKILL;
								action->PLAYERKILL.nump=nump;
								action->PLAYERKILL.nump_frags=player[nump].lapcounter;
								action->PLAYERKILL.creator=res;
								action->PLAYERKILL.creator_frags=player[res].lapcounter;
								Server_SendActions(action,0);
								ChatAddKillMessage(res,nump);
							}
						}

						if (play->Flag && LocalComputerIsServer)
						{
							if ((play->Flag<NumObjects) && (gameobj[play->Flag].ProcPtr==&cFLAG))
							{
								Omonster *flagobj;

								flagobj=(Omonster *)(&gameobj[play->Flag]);
								flagobj->var1=0;
								flagobj->state=sSLEEP;
							}

							play->Flag=0;
						}
					} else {	//gamecapturetheflag
						play->lives--;	//is this rite??????????
					}
				}	//nrg<0

				if (play->character==mSPAZ)
				{
					if (RandFac(1)==0)
						PlaySample(play->xpos,play->ypos,sSPAZSOUNDS_AUTSCH1,0,0);
					else
						PlaySample(play->xpos,play->ypos,sSPAZSOUNDS_AUTSCH2,0,0);
				}
				else
				if (play->character==mJAZZ)
					PlaySample(play->xpos,play->ypos,JAZZHURTSOUNDS[RandFac(7)],0,0);

			}

			play->hit=HITTIME;	//INVINCIBLE TIME! SHOULD FLICKER...
		}
	}

//	player[nump].hpole=0;
//	player[nump].vpole=0;

	return(abletohit);
} //HitPlayer()



/*
	hitplayer, and throw him around a bit.
	bemoved counter is set, which should countdown

*/
void HitThrowPlayer(int nump,int numobj,int xspeed,int yspeed)
{

	ASSERT(nump<MAXPLAYERS);
	HitPlayer(nump,numobj);

	player[nump].xspeed=xspeed;
	player[nump].xacc=xspeed/64;
	player[nump].yspeed=yspeed;
	player[nump].yacc=0;
	player[nump].bemoved=AISPEED/2;
	player[nump].idletime=0;		//remove idle (looks stupid)

} //HitThrowPlayer()





static int ricosamps[4]={sAMMO_BUL1,sAMMO_BULFL1,sAMMO_BULFL2,sAMMO_BULFL3};

void HitObject(int num,int nump,int numbullet)
{
	Tplayer *play;
	Omonster *mons;
	Omonster *mons2;
	Obullets *bull;
	int nobulletexplode=0;
	int t,i;	//gen counter
	int killit;
	int temp;
	int bullforce;
	int sample;
	int res;


	killit=0; //no particles

	if (nump>=PLAYSLOT) {
		play=&player[nump-PLAYSLOT];
	} else {
		play=NULL;
	}

	if (numbullet>0) {
		bull=(Obullets *)(&gameobj[numbullet]);
		bullforce=bull->force;
	} else {
		if (numbullet<-16384)
		{
			bullforce=-numbullet-16384; //TNT explosion
		} else {
			bullforce=-numbullet;
		}
		bull=(Obullets *)(&gameobj[0]); //at least it isn't NULL!
		FastMemSet(bull,0,128); //fuck that
	}

	mons=(Omonster *)(&gameobj[num]);

	if (mons->state==sKILL) 
		return;//fuck off


	if ((mons->nohit&4)==4) //ricochet flag on!
	{
		if ((bull->type&(bullFLAMER+bullLASER))==0)
		{
			nobulletexplode=1;
			if ((bull->ricos==0) || (bull->lastrico>7))
			{
				bull->xpos-=bull->xspeed; //to move it away
				bull->xspeed=-bull->xspeed;
				bull->xacc=-bull->xacc;
				bull->direction=-bull->direction;
				bull->yspeed=-bull->yspeed+Rand2Fac(32767)*16;
				if (bull->yspeed<-8*65536) bull->yspeed=-8*65536;
				else
				if (bull->yspeed>8*65536) bull->yspeed=8*65536;

				for (t=0;t<8+(int)RandFac(7);t++)
					AddParticleSparks(bull->xpos,bull->ypos,-bull->direction);

				PlaySample(mons->xpos,mons->ypos,ricosamps[RandFac(3)],0,0);
				bull->ricos++;
				bull->lastrico=0;
			};
		} else
		if ((bull->type&bullLASER)==0)	//flamer disable
		{
			if (numbullet>0)
				MyDeleteObject(numbullet);
			//bull->code=NULL;
		}
	}


	//NOHIT=1: totally not hittable (bullets flies on)
	//NOHIT=2: bullet explode, dont do anything to monster
	//NOHIT=3: bullet doesnt hit object, after which SPECIAL objects are processed
	//NOHIT|=4 ricocheting
	//NOHIT|=8 no freezing possible!
	//NOHIT|=16 not affected by blasts!



	if ((mons->nohit&3)==1)
	{
		return;
	}
	else
	if ((mons->nohit&3)==2)
	{
		bull->state=sEXPLODE;
		return;
	}


	if ((mons->energy>0) && ((mons->nohit&3)!=3))
	{
		if (mons->freeze>0)
		{
			if (bullforce>3)
				mons->energy-=bullforce;
			else
				mons->energy-=3;
			mons->freeze=0;
			mons->justhit=FLASHTIME;
			if (mons->energy<=0)
			{
				mons->energy=0;
				cUNFREEZE(num,0);
//				MyDeleteObject(num);
//				bull->state=sEXPLODE;

				mons->state=sKILL;
				DoPoints(nump,num);
			}

		}
		else
		{
			mons->energy-=bullforce;
			mons->justhit=FLASHTIME;
			if (mons->energy<=0)
			{
				mons->energy=0;
				mons->state=sKILL;
				killit=1;

				DoPoints(nump,num);
			}
		}
	}


	if (bull->var4==8)	
	{	//8 goes thru everything (laser)
		nobulletexplode=1;
	}


	if ((mons->objtype&oTYPES)==oSPECIAL)
	{
		sample=0;
		switch (mons->load)
		{
			case aFLAG:
				break;
			case aFRUITPLATFORM:
			case aBOLLPLATFORM:
			case aGRASSPLATFORM:
			case aPINKPLATFORM:
			case aSONICPLATFORM:
			case aSPIKEPLATFORM:
				if (mons->var2==0)
					mons->var4+=(bull->xspeed*bullforce)/256;
				break;

			case aSPIKEBOLL:
				mons->var4+=(bull->xspeed*bullforce)/256;

				mons->energy-=bullforce;
				if (mons->energy<=0)
					mons->state=sKILL; //fall apart!
				break;

			case aSTOMPSCENERY:
				if (((bull->load==aPLAYERLASER) || (bull->load==aBOMB) ||(numbullet<-16384)) &&	//tnt explosion
					(mons->state==sSLEEP))
				{
					mons->energy=0;
					mons->state=sKILL;
				};
				nobulletexplode=1;
				break;

			case aDESTRUCTSCENERY:
				if (mons->state!=sDONE)
				{
					temp=GetEventParm(mons->xorg/(32*65536),mons->yorg/(32*65536),15,4);

					if (((temp==0) && (mons->curanim!=(int)AnimBase[mDESTSCEN]+mDESTSCEN_48)) ||
						(bull->load==aPLAYERLASER) || 
						(numbullet<-16384))
					{
						mons->energy-=bullforce;

						if (mons->energy>0)
							mons->state=sHIT;
						else
						{
							mons->state=sKILL;
							DoPoints(nump,num);
						}
					}
					else
					if (numbullet>0)
					{
						if (temp==bull->var4)
						{
							mons->energy-=bullforce;
							if (mons->energy>0)
								mons->state=sHIT;
							else
							{
								mons->state=sKILL;

								DoPoints(nump,num);
							}
						}
					}

					//PlaySample(mons->xpos,mons->ypos,sample,0,0);
					bull->state=sEXPLODE;
				};
				return;
				break;

			case aDESTRUCTSCENERYBOMB:
				if (mons->state!=sDONE)
				{
					if ((bull->load==aBOMB) || 
						(bull->load==aPLAYERLASER) ||
						(numbullet<-16384))	//hits everything thing ng
					{
						mons->energy--;
						if (mons->energy>0)
							mons->state=sHIT;
						else
						{
							mons->state=sKILL;
							DoPoints(nump,num);
						}
					}
					//PlaySample(mons->xpos,mons->ypos,sample,0,0);
					bull->state=sEXPLODE;
				}
				return;
				break;

			case aCOLLAPSESCENERY:
				if ((bull->load==aBOMB) || 
					(bull->load==aPLAYERLASER) ||
					(numbullet<-16384))	//hits everything thing ng
				{
					if (mons->state==sSLEEP)
					{
						if (mons->counter==0)
							mons->counter=1;
					}
					bull->state=sEXPLODE;
				}			
				else
				nobulletexplode=1;
				
				return;
				break;

//			case aGUN1AMMO15:
			case aGUN2AMMO15:
			case aGUN3AMMO15:
			case aGUN4AMMO15:
			case aGUN5AMMO15:
			case aGUN6AMMO15: //remove for old monitor
				mons->state=sACTION;
				mons->nohit=1; //dont hit now ;-)
/*				temp=mons->var4;//1+mons->load-aGUN2AMMO15;
				if (play->ammo[temp]==0)
					play->firetype=temp;
				play->ammo[temp]+=15;
				killit=2;
*/				sample=sCOMMON_GLASS2;
				break;

/*			case aGUN6AMMO15:
				temp=5;//1+mons->load-aGUN2AMMO15;
				if (play->ammo[temp]==0)
					play->firetype=temp;
				play->ammo[temp]+=15*32;
				killit=2;
				sample=sCOMMON_GLASS2;
				break;
*/
			case aREDSPRING:
			case aGREENSPRING:
			case aBLUESPRING:
			case aFROZENGREENSPRING:
				if ((bull->type&bullFLAMER)==bullFLAMER)
				{
					mons->freeze=0;
					mons->nohit=1;
				};				
			
				break;		

			case aTNT:
				if (mons->state!=sEXPLODE)
				{
					mons->counter=0;
					mons->state=sEXPLODE; //explode at once
					killit=0; //no extra, no particles flying off
				};
				break;

			case aGUN1POWER:
			case aGUN2POWER:
			case aGUN3POWER:
			case aGUN4POWER:
			case aGUN5POWER:
			case aTNTPOWER:
			case aGUN8POWER:
			case aGUN9POWER:
				if (nump>=PLAYSLOT)
				{
					temp=mons->var4;
					if (play->gunpower[temp]==0)
					{
						play->gunpower[temp]=1;
						play->firetype=temp;
						if (play->ammo[temp]==0)
							play->ammo[temp]+=20;

						temp=FindEmptyMessage(nump,1);
						play->Display[temp].State=1;
						EStringGet(play->Display[temp].Text,sizeof(play->Display[temp]),strPOWERUP);
						play->Display[temp].Counter=0;
					}
					else
					{
						play->ammo[temp]+=20;
						if (MultiPlayerType>gameCOOPERATIVE)
						{
							if (play->ammo[temp]>50)
								play->ammo[temp]=50;
						}
					}
					killit=2;
					sample=sCOMMON_GLASS2;
					mons->nohit=1;
				};
				break;

			case aGUN6POWER:
				if (nump>=PLAYSLOT)
				{
					temp=5;
					if (play->gunpower[temp]==0)
					{
						play->gunpower[temp]=1;
						play->firetype=temp;
						if (play->ammo[temp]==0)
							play->ammo[temp]+=20*32;

						temp=FindEmptyMessage(nump,1);
						play->Display[temp].State=1;
						EStringGet(play->Display[temp].Text,sizeof(play->Display[temp].Text),strPOWERUP);
						play->Display[temp].Counter=0;
					}
					else
					{
						play->ammo[temp]+=20*32;

						if (MultiPlayerType>gameCOOPERATIVE)
						{
							if (play->ammo[temp]>50*32)
								play->ammo[temp]=50*32;
						}
					}
					killit=2;
					sample=sCOMMON_GLASS2;
					mons->nohit=1;
				}
				break;

			case aCHESHIRE1:
				if (mons->state==sSLEEP)
					mons->state=sEXTRA; //very quick fadeout
				nobulletexplode=1;
				break;

			case aBIGROCK:
			case aBIGBOX:
				if ((numbullet>0) && 
					(bull->type&(bullLASER+bullFLAMER)) &&
					(mons->freeze>0))
				{
					mons->freeze=0;

				} //melt bigblock only with laser or flamer
				break;


			case aSKELETON: 
				//this stuff must be done here, since the
				//flying bone dir and such depends on the bullet

				t=RandFac(1)+1;
				do{
					i=AddObject(mons->xpos,mons->ypos-RandFac(32767)*32,aSHARD,0);
					if (i>0)
					{
						mons2=(Omonster *)&gameobj[i];
						mons2->ProcPtr=&cBONE;
						mons2->curanim=AnimBase[mSKELETON]+mSKELETON_BONEZ;
						if (numbullet>0)
						{
							mons2->xspeed=bull->direction*(RandFac(32767)*16);
							mons2->direction=bull->direction;
						}
						else
							mons2->xspeed=(Rand2Fac(32767)*16);
						mons2->yspeed=(-16384+RandFac(32767))*16;

						mons2->phase=((int)RandFac(31))%anims[mons2->curanim].numframes;
						mons2->state=sACTION;	//skip random randpart
						switch ((RandFac(3))&7)
						{
							case 0:
								mons2->var6=sSKELETON_BONE1;
								break;
							case 1:
								mons2->var6=sSKELETON_BONE2;
								break;
							case 2:
							case 6:
								mons2->var6=sSKELETON_BONE3;
								break;
							case 3:
								mons2->var6=sSKELETON_BONE5;
								break;
							case 4:
								mons2->var6=sSKELETON_BONE6;
								break;

							default:
							case 7:
							case 5:
								mons2->var6=sSKELETON_BONE7;
								break;
						}; //switch
					}	//i>0
					t--;
				} while (t>=0);
				break;

			case aSUPERREDGEM: 
				if (mons->state!=sDONE)
				{
					killit=2;
					mons->state=sACTION;
					mons->nohit=1;
				}
				break;

			case aGUNCRATE: //wood box
			case aGEMCRATE:
			case aCARROTCRATE:
			case aBOMBCRATE:
			case aSPRINGCRATE:
			case a1UPCRATE:
				mons->state=sACTION;
				killit=0;
				mons->nohit=1;
				DoPoints(nump,num);
				break;

			case aBIRDCAGE:
				if (nump>=PLAYSLOT)
				{
					if (play->bird)
					{
						if (mons->var1==-1) //not turned into birdy yet
						{
							mons->state=sSLEEP;
							killit=0;
							nobulletexplode=1;
						}
					}
					else
					if (mons->state==sSLEEP) //only when it's a cage
					{
						DoPoints(nump,num);
						mons->nohit=1;
						mons->state=sACTION; 
						mons->var1=nump;
						play->bird=1;//+num;
						killit=0; //dont display crashing cage
					}
				}
				break;

			case aENDOFLEVELPOST: //EOL end of level
				if (nump>=PLAYSLOT)
				{
					if (MultiPlayerType==gameTREASURE)
					{
						if (play->gem[1]<level.extra)
							break; //not enough gems, loser!
						else
						{
							if (play->lastlapfinish==0)
							{	//not finished yet
								play->lastlapfinish=gamecounter;
							}
							if (play->finished==0)
								play->finished=1;
						}
					}

					mons->state=sACTION;
					if (level.finish==0)
					{
						if ((play->energy>0) && (play->lives>0))
						{
							if (mons->var1)
								strcpy(level.loadname, LevHdr.SecretLevel);
							else
								strcpy(level.loadname, LevHdr.NextLevel);
						}

						level.finish=1;
						level.finishcounter=1;
					}
				}
				killit=0;
				break;

			case aBONUSLEVELPOST:	
				if (nump>=PLAYSLOT)
				{
					mons->state=sACTION;
					if (level.finish==0)
					{
						if ((play->energy>0) && (play->lives>0))
							strcpy(level.loadname, LevHdr.SecretLevel);
						level.finish=1;
						level.finishcounter=1;
					}
				}
				killit=0;
				break;

			case aSAVEPOST:
				if ((mons->state==sSLEEP) ||
					((MultiPlayerType==gameCOOPERATIVE) && (mons->state==sDONE)))
				{
					DoPoints(nump,num);

					mons->state=sACTION;
					mons->special=nump;
					mons->nohit=3;	//not hittable anymore
//					if (play->character==mSPAZ)
//						PlaySample(play->xpos,play->ypos,sSPAZSOUNDS_HAHAHA,0,0);
				}
				killit=0;				
				break;

			case aFIRESHIELD:
				if (nump>=PLAYSLOT)
				{
					play->shield=AISPEED*40;
					play->shieldtype=1;
					play->firetype=0;
					killit=2;
					PlaySample(play->xpos,play->ypos,sCOMMON_GLASS2,0,0);
					sample=sCOMMON_SHIELD1;
				}
				break;

			case aWATERSHIELD:
				if (nump>=PLAYSLOT)
				{
					play->shield=AISPEED*40;
					play->shieldtype=2;
					play->firetype=0;
					killit=2;
					PlaySample(play->xpos,play->ypos,sCOMMON_GLASS2,0,0);
					sample=sCOMMON_SHIELD1;
				}
				break;

			case aLIGHTSHIELD:
				if (nump>=PLAYSLOT)
				{
					play->shield=AISPEED*40;
					play->shieldtype=3;
					play->firetype=0;
					killit=2;
					PlaySample(play->xpos,play->ypos,sCOMMON_GLASS2,0,0);
					sample=sCOMMON_SHIELD1;
				}
				break;

			case aLASERSHIELD:
				if (nump>=PLAYSLOT)
				{
					play->shield=AISPEED*40;
					play->shieldtype=4;
					play->firetype=0;
					killit=2;
					sample=sCOMMON_GLASS2;
				}
				break;


			case aTURTLESHELL:
				if ((numbullet>0) && ((bull->type&(bullFLAMER+bullLASER))==0))
				{
					if (ABS(mons->xspeed-bull->xspeed)>65536)
						mons->counter=0;

					mons->energy=3;	//restore energy
					mons->xspeed=bull->xspeed;
					mons->state=sACTION;	
					killit=0;
					sample=sTURTLE_HITSHELL;
				}
				break;

			case aROCK:
				if (bull->force)
				{
					if (mons->state==sDONE)
						mons->state=sFLY;
					mons->xspeed+=(bull->xspeed*bull->force)/8;
					if (mons->xspeed<0)
						mons->xacc=-2048*2;
					else
						mons->xacc=2048*2;
				}

				break;

			case aNORMTURTLE:
				if (mons->var4==0)
				{
					//special kill type (shell flies)
					if (mons->state==sKILL)
					{
						mons->creator=nump; //player killed this!
						//mons->xspeed=bull->xspeed;
						killit=-1;
					}
					break;
				}
				//nobreak: ricochet fallthrough!!!
			case aHREDSPRING:
			case aHGREENSPRING:
			case aHBLUESPRING:
				if (numbullet>0)
				if ((bull->type&(bullFLAMER+bullLASER))==0)
				{
					nobulletexplode=1;

					if ((bull->ricos==0) || (bull->lastrico>7))
					{
						bull->xpos-=bull->xspeed; //to move it away
						bull->xspeed=-bull->xspeed;
						bull->xacc=-bull->xacc;
						bull->direction=-bull->direction;
						bull->yspeed=-bull->yspeed+Rand2Fac(32767)*16;
						if (bull->yspeed<-8*65536) bull->yspeed=-8*65536;
						else
						if (bull->yspeed>8*65536) bull->yspeed=8*65536;

						for (t=0;t<8+(int)RandFac(7);t++)
							AddParticleSparks(bull->xpos,bull->ypos,-bull->direction);

						PlaySample(mons->xpos,mons->ypos,ricosamps[RandFac(3)],0,0);
						bull->ricos++;
						bull->lastrico=0;
					} else
						bull->state=sEXPLODE;
				}
				break;
					
			
			case aTRIGGERCRATE:
			case aGUNBARREL:
			case aGEMBARREL:
			case aCARROTBARREL:
			case a1UPBARREL:
				if (((bull->type&bullLASER)==bullLASER) || 
					(numbullet<-16384))	//hits everything thing ng
				{	//destroy this object, because we're hitting it with heavy ammo
					if (mons->freeze>0)
					{
						cUNFREEZE(num,1);
						//mons->code=NULL;
						MyDeleteObject(num);
					}
					else
						mons->state=sACTION; //! death
					DoPoints(nump,num);
					sample=sCOMMON_GLASS2;

				} else
				if  ((numbullet>0) &&
					((bull->type&bullFLAMER)!=bullFLAMER))	//not for flamer
				{	//ricochet bullets
					nobulletexplode=1;

					if ((bull->ricos==0) || (bull->lastrico>7))
					{
						bull->xpos-=bull->xspeed; //to move it away
						bull->xspeed=-bull->xspeed;
						bull->xacc=-bull->xacc;
						bull->direction=-bull->direction;
						bull->yspeed=-bull->yspeed+Rand2Fac(32767)*16;
						if (bull->yspeed<-8*65536) bull->yspeed=-8*65536;
						else
						if (bull->yspeed>8*65536) bull->yspeed=8*65536;

						for (t=0;t<8+(int)RandFac(7);t++)
							AddParticleSparks(bull->xpos,bull->ypos,-bull->direction);

						PlaySample(mons->xpos,mons->ypos,ricosamps[RandFac(3)],0,0);
						bull->ricos++;
						bull->lastrico=0;
					} else
						bull->state=sEXPLODE;
				}
				break;

			case aFLOATSUCKER:
				if (mons->state==sFLOAT)
				{
					mons->state=sHIT;
					mons->xspeed=bull->xspeed/2;
					mons->yspeed=32768;
					mons->counter=0;
					mons->phase=0;
					mons->nohit=1;
					//mons->energy=1;
				}
				break;

			case aFLOATLIZARD:
				if (mons->var6==1)
				{
					mons->var6=0;
					t=AddObject(mons->xpos,mons->ypos-4*65536,aCOPTER,0);
					if (t>0)
					{
						mons2=(Omonster *)(&gameobj[t]);
						mons2->direction=mons->direction;
						mons2->state=sFLY;
						mons2->var4=mons2->var5=mons->var5;
						mons2->yspeed=-16384;
						mons2->xspeed=mons->direction*16384; //slowly
					}
				}

				if ((mons->state!=sKILL) && (mons->state!=sFALL))
				{
					mons->state=sFALL;
					//mons->nohit=1;
					mons->xspeed=mons->direction*65536;
					mons->yspeed=32768;
					mons->counter=0;
					mons->phase=0;
				}
				break;

			case aCATERPILLAR:
				mons->state=sHIT;
				break;

			case aDOGGYDOGG: //hittin dog
				if (mons->state!=sKILL)
				{
					if (mons->state!=sACTION)
					{
						sample=sDOG_AGRESSIV;
						mons->state=sACTION;
					}
					mons->freeze=0; //?
					mons->counter=gamecounter&7;	//aligning anim
					mons->var2=255;
					mons->var1=-1;
					if (numbullet>0)
					{
						if (bull->xspeed>0)
							mons->direction=-1;
						else
							mons->direction=1;
					}
				}
				break;

			case aJUNGLEPOLE:
			case aDIAMONDUSPOLE:
			case aCARROTUSPOLE:
			case aPSYCHPOLE:
			case aROTSMALLTREE:
				if ((mons->state==sSLEEP) &&
					(nump>=PLAYSLOT))
				{
					mons->state=sHIT;
					if (numbullet>0)
					{
						if (bull->direction>0)
							mons->var2=-8;
						else
							mons->var2=8;
					}
					else
						mons->var2=-8;


					if (mons->ypos>level.waterlevel)
						mons->var2/=4;

					mons->creator=nump; //for now

					DoPoints(nump,num);

					if (mons->load==aROTSMALLTREE)
						PlaySample(mons->xpos,mons->ypos,sSMALTREE_FALL,0,0);
				}
				break;

			case aBIRDMORPHMONITOR:
			case aMORPHMONITOR:
				if (nump>=PLAYSLOT)
				{
					mons->state=sKILL;
					killit=2;
					
					PlaySample(play->xpos,play->ypos,sCOMMON_GLASS2,0,0);
					PlaySample(play->xpos,play->ypos,sCOMMON_HOLYFLUT,0,0);
					sample=sCOMMON_BUBBLGN1;

					play->morph=AISPEED*30;
					if (mons->load==aBIRDMORPHMONITOR)
						ChangeCharacter(nump-PLAYSLOT,mBIRD);
					else
					{
            			if (player[nump-PLAYSLOT].character==mJAZZ)
	            			ChangeCharacter(nump-PLAYSLOT,mSPAZ);
						else
							ChangeCharacter(nump-PLAYSLOT,mJAZZ);
					}
				}
				break;




//BOSSES
			case aQUEENBOSS:
				if ((mons->state==sIDLE) || //ricocheting bullets when shield is up
					(mons->state==sHIT))
				{
					if (numbullet>0)
					if ((bull->type&(bullLASER+bullFLAMER))==0)
					{
						nobulletexplode=1;

						if ((bull->ricos==0) || (bull->lastrico>7))
						{
							bull->xpos-=bull->xspeed; //to move it away
							bull->xspeed=-bull->xspeed;
							bull->xacc=-bull->xacc;
							bull->direction=-bull->direction;
							bull->yspeed=-bull->yspeed+Rand2Fac(32767)*16;
							if (bull->yspeed<-8*65536) bull->yspeed=-8*65536;
							else
							if (bull->yspeed>8*65536) bull->yspeed=8*65536;

							for (t=0;t<8+(int)RandFac(7);t++)
								AddParticleSparks(bull->xpos,bull->ypos,-bull->direction);
						
							PlaySample(mons->xpos,mons->ypos,ricosamps[RandFac(3)],0,0);
							bull->ricos++;
							bull->lastrico=0;
						} else
							bull->state=sEXPLODE;
					}
				}
				else
				{
					mons->var6++;	//number of times hit!
					mons->justhit=FLASHTIME;	//dont for the queen???
				}
				break;


			case aROBOT:
				//this stuff must be done here, since the
				//flying parts dir and such depends on the bullet

				if (numbullet>0)
					mons->var6=mons->direction=-bull->direction;					
				else
					mons->var6=0;

				if (mons->energy<10)
					mons->var5=4+RandFac(7);
				else
				if (mons->energy<20)
					mons->var5=4+RandFac(3);
				else
					mons->var5=2+RandFac(3);
				break;

			case aUTERUSEL:
				if (numbullet>0)
				{
					//mons->xspeed=bull->xspeed;
					if (mons->energy<=0)
					{
						mons->state=sKILL;
						DoPoints(nump,num);
					}
					else					
						mons->state=sHIT;
					mons->counter=AISPEED*2;
				}
				killit=0;
				break;

			case aUTERUS: //boss
				if (mons->state!=sEXTRA)	//hitting when still bollsrotating
				{
					mons->energy=loadobj[aUTERUS].energy;
					if (mons->state==sFLY)
						mons->state=sATTACK;
				}
				else
				if ((numbullet>0) && (mons->nohit==0))
				{
					mons->energy-=bullforce;
					if (mons->freeze)
					{
						mons->energy--;
						mons->freeze=0;
					}
					mons->justhit=FLASHTIME;
					if (mons->energy<=0)
					{
						mons->energy=0;
						mons->counter=0;
						mons->state=sDONE;					
						DoPoints(nump,num);
					}
				}
				killit=0; //DONT DISPLAY PARTICLES: THIS THING IS NOT A NORM SPRITE!!!
				break;
		
	
			case aDEVILDEVAN:
				bull->state=sEXPLODE;
				if (mons->var1>0)
				{

					if ((numbullet>0) && bull->freeze)
					{
						mons->freeze=AISPEED/2;

						if (mons->state!=sFREEZE)
							mons->oldstate=mons->state;
						mons->state=sFREEZE;
					}
					else
					{
						if (mons->freeze>0)
						{
							//shooting while frozen!
							if (bullforce>3)
								mons->var1-=bullforce;
							else
								mons->var1-=3;
							mons->freeze=0;
						}
						else
						{	//not frozen!
							mons->var1-=bullforce;
						}
					}

					mons->justhit=FLASHTIME;
					mons->energy=mons->var1/2;
					if (mons->energy<=0)
					{
						if (mons->var8==0)
						{
							mons->state=sWAKE;
							mons->phase=0;
							mons->counter=0;
							mons->var8=1;	//second phase
							mons->var1=200;
							mons->energy=mons->var1/4;
							killit=0;
						}
						else
						{
							mons->energy=0;
							mons->counter=0;
							mons->state=sDONE;
							mons->curanim=AnimBase[mDEVILDEVAN]+mDEVILDEVAN_FLY;
							mons->nohit=3;
							DoPoints(PLAYSLOT+nump,num);
						}
					}
				}
				break;


			case aBILSYBOSS:
			case aSONICBOSS:
			case aTUFBOSS:
			case aBUBBA:
			case aROCKTURT:
				if (mons->energy>=0)
				{
					if (numbullet>0)
					{
						mons->energy-=(char)bull->force;

						if (mons->freeze>0)	//shooting while frozen!
						{
							cUNFREEZE(num,0);							
							mons->energy--;	//extra hitpoint
							mons->freeze=0;
						}
					}
					else
						mons->energy--;

					mons->justhit=FLASHTIME;
					if (mons->energy<0)
					{
						mons->energy=-1;
						mons->counter=0;
						mons->state=sDONE;					
						killit=2;
					}
				}
				break;

  
			default:
				break;
		} //end switch

		if (sample)
			PlaySample(mons->xpos,mons->ypos,sample,0,0);
	}


	if (killit)
	{
		DoPoints(nump,num);

//KILLIT==1 : BOTH (EXTRA+EXPLOSION
		if ((killit<=1) && (nump>0))
		{
			if (numbullet>=0)
			{
				if (bull->curanim==(int)AnimBase[mAMMO]+mAMMO_GUN1)
					AddExtra(nump,num,5);
				else
					AddExtra(nump,num,10);
			} else
				AddExtra(nump,num,10);
		}

		if (killit>=1)
		{
			if ((bull->type&bullFLAMER)==0)
				AddParticlePixelExplosion(num,0);
			else
			{
				PlaySample(mons->xpos,mons->ypos,sCOMMON_BURN,0,0);
				AddParticlePixelExplosion(num,1);
			}
			
		}
	}

	if ((!nobulletexplode) && (numbullet>0))
	{
		bull->state=sEXPLODE;
		if (bull->load==aPLAYERBULLETC3)
		{	//electric shield

			PlaySample(bull->xpos,bull->ypos,sCOMMON_ELECTRICHIT,0,0);

			t=1+RandFac(7);
			do
			{
				res=AddObject(bull->xpos,bull->ypos,aSHARD,0);//nump+PLAYSLOT);
				if (res>0)
				{
					gameobj[res].curanim=AnimBase[mAMMO]+mAMMO_SPARK1;
					gameobj[res].light=1;
					gameobj[res].lighttype=2;
				}
				else 
					break;
			} while (--t);
		}
		else
		if (bull->load==aPLAYERBULLETC1)
		{
			t=1+RandFac(7);
			do
			{
				res=AddObject(bull->xpos,bull->ypos,aBOUNCEONCE,0);//nump+PLAYSLOT);
				if (res>0)
				{
					gameobj[res].curanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
					gameobj[res].light=1;
					gameobj[res].lighttype=2;
				} else break;
			} while (--t);
		}
	}
} //HitObject()





/*
	do the blast of an explosion.
	calculate speeds and damage based on the distance from the player
	to the blast
*/
void DoBlast(int num,int x,int y,int nump,int forceradius)
{
	int t,dx,dy,tdx,tdy,dist;
	int hurtplayer;

	if ((gameobj[num].creator>=PLAYSLOT) && (player[gameobj[num].creator-PLAYSLOT].flicker))
		return;

	if ((nump>=PLAYSLOT) &&
		(forceradius>0) &&
		(MultiPlayerType!=gameRACE) &&
		(MultiPlayerType!=gameCOOPERATIVE))
		hurtplayer=1;
	else
		hurtplayer=0;


	for (t=0;t<MAXPLAYERS;t++)
	if (player[t].Active)
	{
		if ((player[t].flicker) && ((MultiPlayerType==gameBATTLE) || (MultiPlayerType==gameTREASURE) || (MultiPlayerType==gameCAPTURE)))
			continue;

		dx=(x-player[t].xpos);
		tdx=dx/65536;
		dy=(y-player[t].ypos);
		tdy=dy/65536;

		dist=tdx*tdx+tdy*tdy;

		if (dist<ABS(forceradius))
		{
			dx/=2;
			if (dx<-8*65536)
				dx=-8*65536;
			else
			if (dx>8*65536)
				dx=8*65536;
			player[t].xspeed=-dx;

			dy/=2;
			if (dy<-12*65536)
				dy=-12*65536;
			else
			if (dy>12*65536)
				dy=12*65536;
			player[t].yspeed=-dy;

			if ((dist<48*48) &&
				hurtplayer &&
				(t!=nump-PLAYSLOT))
				HitPlayer(t,nump);

			player[t].bemoved=30;
			player[t].idletime=0;
			player[t].spring=0;
		}
	}
} //DoBlast()


/*
	DoFullBlast.
	as DoBlast, but also affects objects

	warning: some objects dont like this, that's why this isn't used
	much (or at all).
	you need to design objects with this in mind, because the XSPEED and 
	YSPEED of objects can suddenly change and you don't want objects
	to start running around really fast.

	objects also cannot detect well if they're hit. Should add some kind
	of logic for this (Save oldstate, set state to THROW or so, and then
	do special AI logic for objects affected)
*/
void DoFullBlast(int num,int x,int y,int nump,int forceradius)
{
	int t,dx,dy,tdx,tdy,dist;
	int force;
	int hurtplayer;
	int mindistance,minobject;

	if ((gameobj[num].creator>=PLAYSLOT) && (player[gameobj[num].creator-PLAYSLOT].flicker))
		return;

	if ((nump>=PLAYSLOT) &&
		(forceradius>0) &&
		(MultiPlayerType!=gameRACE) &&
		(MultiPlayerType!=gameCOOPERATIVE))
		hurtplayer=1;
	else
		hurtplayer=0;


	for (t=0;t<MAXPLAYERS;t++)
	if (player[t].Active)
	{
		if ((player[t].flicker) && ((MultiPlayerType==gameBATTLE) || (MultiPlayerType==gameTREASURE) || (MultiPlayerType==gameCAPTURE)))
			continue;

		if ((MultiPlayerType==gameCAPTURE) && (gameobj[num].creator>=PLAYSLOT) && (player[t].Team==player[gameobj[num].creator-PLAYSLOT].Team))
			continue;

		dx=(x-player[t].xpos);
		tdx=dx/65536;
		dy=(y-player[t].ypos);
		tdy=dy/65536;

		dist=tdx*tdx+tdy*tdy;

		if (dist<ABS(forceradius))
		{
			dx/=2;
			if (dx<-8*65536)
				dx=-8*65536;
			else
			if (dx>8*65536)
				dx=8*65536;
			player[t].xspeed=-dx;

			dy/=2;
			if (dy<-12*65536)
				dy=-12*65536;
			else
			if (dy>12*65536)
				dy=12*65536;
			player[t].yspeed=-dy;

			if ((dist<48*48) &&
				hurtplayer && (t!=nump-PLAYSLOT))
				HitPlayer(t,0);

			player[t].bemoved=30;
			player[t].idletime=0;
			player[t].spring=0;
		}
	}

	minobject=0;
	mindistance=ABS(forceradius);

	for (t=1;t<NumObjects;t++)
	if ((gameobj[t].ProcPtr) && ((gameobj[t].nohit&16)!=16))
	{
		dx=(x-gameobj[t].xpos);
		tdx=dx/65536;
		dy=(y-gameobj[t].ypos);
		tdy=dy/65536;

		dist=tdx*tdx+tdy*tdy;

		if (dist<ABS(forceradius))
		{
			dx/=2;
			if (dx<-6*65536)
				dx=-6*65536;
			else
			if (dx>6*65536)
				dx=6*65536;
			gameobj[t].xspeed=-dx;

			dy/=2;
			if (dy<-6*65536)
				dy=-6*65536;
			else
			if (dy>6*65536)
				dy=6*65536;
			gameobj[t].yspeed=-dy;

			force=1+((ABS(forceradius)-dist)*8)/ABS(forceradius);

			if (gameobj[num].load==aTNT)
			{
				if (gameobj[t].load==aTNT)
				{
					if (dist<mindistance)
					{
						minobject=t;
						mindistance=dist;
					}
				}
				else
				{
					HitObject(t,nump,-16384-force);
				}
			}
			else
			{
				HitObject(t,nump,-force);	//nump is already in scaled PLAYSLOT etc
			}
		}
	}

	if (minobject)	//trigger only nearest TNT!
	{
		HitObject(minobject,nump,-16384-4);	//nump is already in scaled PLAYSLOT etc
	}
} //DoFullBlast()













