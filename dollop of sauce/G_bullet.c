/*
	G_BULLET.C

	
	bullet related routines 
*/
#include "G_Bullet.h"
#include "Random.h"
#include "E_Loaders.h"
#include "E_Sound.h"
#include "E_Graphics.h"
#include "G_Objects.h"
#include "G_AiTypes.h"
#include "Globals.h"
#include "Grade.h"
#include "N_Main.h"
#include "G_Ai.h"
#include "G_Hit.h"
#include "G_Collision.h"

/*
	add an explosion at a certain position.
	no checking
*/
void AddExplosion(int rx,int ry,int anim)
{
	int res;

	res=AddObject(rx,ry,aEXPLOSION,0);
	if (res>0)
		gameobj[res].curanim=anim;

} //AddExplosion



/*
	show little flare sprite at gun exhaust point of
	this character. calls AddSprite to put it at all
	local viewports
*/
void GunFlare(int nump)
{
	Tplayer *play;
	int rx,ry;
	Tframes *frame;
	int fr;


	play=&player[nump];

	frame=&frames[play->curframe];

	if (play->direction<0)
		rx=play->xpos-((frame->tx2-frame->tx3)*65536);
	else
	   	rx=play->xpos+((frame->tx2-frame->tx3)*65536);

	ry=play->ypos+((frame->ty2-frame->ty3)*65536);

	if (play->fly<0)
		ry+=-8*65536+sintable(gamecounter*12)*8;

	fr=anims[AnimBase[mAMMO]+mAMMO_GUN0SHOT].framelist[0];

	AddSprite(rx,ry,SPRITELAYER,play->direction,spriteNORMAL,0,fr);

} //GunFlare()





static void FixupBullet(int nump,int res,int firetype,int uid,int subnum)
{
	Tplayer *play;
	Obullets *bull;
	int l;

	play=&player[nump];

	if (res>0)
	{
		bull=(Obullets *)(&gameobj[res]);

		if (play->swim)
		{
			if (play->yspeed<-2*65536)
				bull->yspeed=-2*65536-bull->xspeed;
			else
			if (play->yspeed>2*65536)
				bull->yspeed=2*65536+bull->xspeed;
		}

		if ((play->firedirection==8) && bull->upanim)//firing up!
		{
			l=(bull->lifetime*2)/3;
			bull->yspeed=-bull->xspeed;
			bull->yacc=-bull->xacc;
			bull->xspeed=0;
			bull->xacc=0;
			bull->curanim=(Word16)bull->upanim;
			bull->direction=0; //!
			bull->lifetime=l;
		}
		else
		{
			bull->direction=play->direction;	
			bull->xspeed=play->direction*bull->xspeed;
			bull->xacc=play->direction*bull->xacc;

			bull->pxspeed=play->xspeed;
			bull->pyspeed=0;//play->yspeed/8;


			if (bull->pxspeed>8*65536) bull->pxspeed=8*65536;
			else
			if (bull->pxspeed<-8*65536) bull->pxspeed=-8*65536;

		}
		if (uid)
			UID[res]=uid;
	}
}	//FixUpBullet()






/*
	add special rocket bullets, which are 3 rockets flying 
	in slightly different angles
*/
static void Add3Bullet(int rx,int ry,int nump,int uid)
{
	int res;
	Obullets *obj;
	Tplayer *play;
	int gun;

	ASSERT(nump<MAXPLAYERS);
	play=&player[nump];


	if (play->gunpower[4]==1)
		gun=aPLAYERBULLETP5;
	else
		gun=aPLAYERBULLET5;


	res=AddObject(rx,ry,gun,PLAYSLOT+nump);
	if (res>0)
	{
        obj=(Obullets *)(&gameobj[res]);

		if (play->firedirection==8)
		{
			if (gun==aPLAYERBULLETP5)
				obj->curanim=AnimBase[mAMMO]+mAMMO_GUN5PU;
			else
				obj->curanim=AnimBase[mAMMO]+mAMMO_GUN5U;

			obj->yspeed=-obj->xspeed;
			obj->yacc=-obj->xacc;
			obj->xspeed=-1*65536;
			obj->xacc=0;
		}
		else
		{
			if (gun==aPLAYERBULLETP5)
				obj->curanim=AnimBase[mAMMO]+mAMMO_GUN5P;
			else
				obj->curanim=AnimBase[mAMMO]+mAMMO_GUN5;

			obj->xspeed=play->direction*obj->xspeed+play->xspeed;
			obj->xacc=play->direction*obj->xacc;
			obj->direction=play->direction;

			if (play->swim)
			{
				if (play->yspeed<-2*65536)
					obj->yspeed=-3*65536-obj->xspeed;
				else
				if (play->yspeed>2*65536)
					obj->yspeed=1*65536+obj->xspeed;
				else
					obj->yspeed=-1*65536;
			} else
				obj->yspeed=-1*65536; 
		}

		if (uid)
			UID[res]=uid;
	}

	if (gun==aPLAYERBULLETP5)
	{
		res=AddObject(rx,ry,gun,PLAYSLOT+nump);
		if (res>0)
		{
			obj=(Obullets *)(&gameobj[res]);
			if (play->firedirection==8)
			{
				obj->curanim=AnimBase[mAMMO]+mAMMO_GUN5PU;
				obj->yspeed=-obj->xspeed;
				obj->yacc=-obj->xacc;
				obj->xspeed=0;
				obj->xacc=0;
				obj->direction=0; //!
			}
			else
			{
				obj->curanim=AnimBase[mAMMO]+mAMMO_GUN5P;
				obj->direction=play->direction;
				obj->xspeed=play->direction*obj->xspeed+play->xspeed;
				obj->xacc=play->direction*obj->xacc;

				if (play->swim)
				{
					if (play->yspeed<-2*65536)
						obj->yspeed=-2*65536-obj->xspeed;
					else
					if (play->yspeed>2*65536)
						obj->yspeed=2*65536+obj->xspeed;
					else
						obj->yspeed=0;
				} else
					obj->yspeed=0;
			}
		}

		if (uid)
			UID[res]=uid;
	}

	res=AddObject(rx,ry,gun,PLAYSLOT+nump);
	if (res>0)
	{
		obj=(Obullets *)(&gameobj[res]);

		if (play->firedirection==8)
		{
			if (gun==aPLAYERBULLETP5)
				obj->curanim=AnimBase[mAMMO]+mAMMO_GUN5PU;
			else
				obj->curanim=AnimBase[mAMMO]+mAMMO_GUN5U;

			obj->yspeed=-obj->xspeed;
			obj->yacc=-obj->xacc;
			obj->xspeed=1*65536;
			obj->xacc=0;
		}
		else
		{
			if (gun==aPLAYERBULLETP5)
				obj->curanim=AnimBase[mAMMO]+mAMMO_GUN5P;
			else
				obj->curanim=AnimBase[mAMMO]+mAMMO_GUN5;

			obj->direction=play->direction;
			obj->xspeed=play->direction*obj->xspeed+play->xspeed;
			obj->xacc=play->direction*obj->xacc;

			if (play->swim)
			{
				if (play->yspeed<-2*65536)
					obj->yspeed=-1*65536-obj->xspeed;
				else
				if (play->yspeed>2*65536)
					obj->yspeed=3*65536+obj->xspeed;
				else
					obj->yspeed=1*65536;
			} else
				obj->yspeed=1*65536; 
		}

		if (uid)
			UID[res]=uid;

	}
} //Add3Bullet





static int bouncesampsn[6]={sAMMO_GUNFLPL,sAMMO_GUNFLP,sAMMO_GUNFLP1,
					sAMMO_GUNFLP2,sAMMO_GUNFLP3,sAMMO_GUNFLP4};
static int bouncesampsp[6]={sAMMO_BMP1,sAMMO_BMP2,sAMMO_BMP3,
					sAMMO_BMP4,sAMMO_BMP5,sAMMO_BMP6};
static int gun0spaz[3]={sAMMO_GUN3PLOP,sAMMO_GUN1,sAMMO_GUN2};
static int icegunn[3]={sAMMO_ICEGUN,sAMMO_ICEGUN2,sAMMO_ICEGUNPU};
static int icegunp[4]={sAMMO_ICEPU1,sAMMO_ICEPU2,sAMMO_ICEPU3,sAMMO_ICEPU4};


static void AddBulletSounds(int nump,int rx,int ry)
{
	Tplayer *play;
	int rnd;
	int power;
	int firetype;


	play=&player[nump];

	if ((play->firetype==0) && (play->shield>0))
		firetype=9+play->shieldtype;
	else
		firetype=play->firetype;

	power=play->gunpower[play->firetype];

	switch (firetype)
	{
		case 0:
			if (power)
			{
				if (play->character==mSPAZ)
				{
					if (RandFac(1))
						PlaySample(rx,ry,sAMMO_FUMP,0,0);
					else
						PlaySample(rx,ry,sAMMO_GUN3PLOP,0,0);
				} else
					PlaySample(rx,ry,sAMMO_FUMP,0,22050);
			}
			else
			{
				if (play->character==mJAZZ)
					PlaySample(rx,ry,sAMMO_GUNJAZZ,0,22050);
				else
				{
					rnd=(RandFac(255))%3;
					PlaySample(rx,ry,gun0spaz[rnd],0,22050);
				}
			}
			break;

		case 1:
			if (power)
			{
				rnd=(RandFac(255))%6;
				if (play->character==mJAZZ)
					PlaySample(rx,ry,bouncesampsp[rnd],0,11025);
				else
					PlaySample(rx,ry,bouncesampsp[rnd],0,22050);
			}
			else
			{
				rnd=(RandFac(255))%6;
				if (play->character==mJAZZ)
					PlaySample(rx,ry,bouncesampsn[rnd],0,11025);
				else
					PlaySample(rx,ry,bouncesampsn[rnd],0,22050);
			}
			break;

		case 2:
			if (power)
			{
				rnd=RandFac(3);
				PlaySample(rx,ry,icegunp[rnd],0,0);
			} 
			else
			{
				rnd=(RandFac(255))%3;
				PlaySample(rx,ry,icegunn[rnd],0,0);
			}
			break;

		case 3:
			if (sndAudio)
				PlaySample(rx,ry,sAMMO_MISSILE,0,0);
			break;

		case 4:
			if (sndAudio)
				PlaySample(rx,ry,sAMMO_LAZRAYS,0,0);
			break;

		case 5:
			//PlaySample(rx,ry,sGUNSM1,0,0);
			break;

		case 6: //TNT!!!!!!!
						
			break;

		case 7:
			PlaySample(rx,ry,sAMMO_GUNVELOCITY,0,0);
			break;

		case 8:
			PlaySample(rx,ry,sAMMO_LASER2+RandFac(1),0,0);
			break;

		case 10:
			if (RandFac(1))
				PlaySample(rx,ry,sAMMO_FIREGUN1A,0,11025-3000);
			else
				PlaySample(rx,ry,sAMMO_FIREGUN2A,0,11025-3000);
			break;

		case 11:
			if (RandFac(1))
				PlaySample(rx,ry,sAMMO_BLUB1,0,0);
			else
				PlaySample(rx,ry,sAMMO_BLUB2,0,0);
			break;

		case 12:
			PlaySample(rx,ry,sCOMMON_ELECTRIC1,0,0);
			break;

		case 13:
			PlaySample(rx,ry,sAMMO_LASER,0,0);
			break;

		default:
			break;
	}; //switch firetype
}	//AddBulletSounds()




int AddRealBullet(int rx,int ry,int nump,int uid,int firetype,int power)
{
	int res;
	Obullets *bull;
	Tplayer *play;
	int rnd;

	play=&player[nump];

	if (MultiPlayerConnection>gameLOCAL)
	{
		//if server:
		Net_AddBullet(nump,firetype,rx,ry);		//not always sent, when this is received

	}	//send over net


	switch (firetype)
	{
		case 0:
			if (power)
				res=AddObject(rx,ry,aPLAYERBULLETP1,PLAYSLOT+nump);
			else
				res=AddObject(rx,ry,aPLAYERBULLET1,PLAYSLOT+nump);
			break;

		case 1:
			if (power)
				res=AddObject(rx,ry,aPLAYERBULLETP2,PLAYSLOT+nump);
			else
				res=AddObject(rx,ry,aPLAYERBULLET2,PLAYSLOT+nump);
			break;

		case 2:
			if (power)
			{
				res=AddObject(rx,ry,aPLAYERBULLETP3,PLAYSLOT+nump);
				if (res>0)
				{
					bull=(Obullets *)(&gameobj[res]);
					bull->var1=0;
					if (uid)
						UID[res]=uid;
				}

				res=AddObject(rx,ry,aPLAYERBULLETP3,PLAYSLOT+nump);
				if (res>0)
				{
					bull=(Obullets *)(&gameobj[res]);
					bull->var1=512; //sinewaved!
					if (uid)
						UID[res]=uid;
				}
			} 
			else
			{
				res=AddObject(rx,ry,aPLAYERBULLET3,PLAYSLOT+nump);
			}
			break;

		case 3:
			if (power)
				res=AddObject(rx,ry,aPLAYERBULLETP4,PLAYSLOT+nump);
			else
				res=AddObject(rx,ry,aPLAYERBULLET4,PLAYSLOT+nump);
			break;

		case 4:
			Add3Bullet(rx,ry,nump,uid);
			res=0;
			break;

		case 5:
			if (play->firedirection==8)
				ry-=8*65536;

			if (power)
				res=AddObject(rx,ry,aPLAYERBULLETP6,PLAYSLOT+nump);
			else
				res=AddObject(rx,ry,aPLAYERBULLET6,PLAYSLOT+nump);

			if (res>0)
			{
				bull=(Obullets *)(&gameobj[res]);
				bull->xspeed=2*65536+(35-play->firespeed)*2048;
			}
			break;

		case 6: //TNT!!!!!!!
			res=AddObject(play->xpos,play->ypos,aTNT,PLAYSLOT+nump);

			if (power && (res>0))
				((Omonster *)(&gameobj[res]))->var1=-128*128;
			break;

		case 7:
//shit bullet, remove
			if (play->firedirection==8)
				res=256;
			else
			if (play->direction==1)
			{
				res=512;
				if (play->fireangle<128)
					play->fireangle=res;
			}
			else
			if (play->direction==-1)
			{
				res=0;
				if (play->fireangle>384)
					play->fireangle=res;
			}

			if (play->fireangle>res)
				play->fireangle-=64;
			else
			if (play->fireangle<res)
				play->fireangle+=64;


			rnd=2;//1+RandFac(3);
			do {

				res=AddObject(rx,ry,aBULLET,PLAYSLOT+nump);
				if (res>0)
				{
					if (uid)
						UID[res]=uid;

					bull=(Obullets *)(&gameobj[res]);

					bull->ProcPtr=&cSPREAD;
					bull->objtype=oPLAYERBULLET;
					bull->force=1;//rnd&1;

					if (play->swim)
					{
						if (play->yspeed<-2*65536)
							bull->yspeed=-2*65536-bull->xspeed;
						else
						if (play->yspeed>2*65536)
							bull->yspeed=2*65536+bull->xspeed;
					}


					res=play->fireangle+Rand2Fac(31);

					power=-256-Rand2Fac(127);
					bull->xspeed=play->xspeed+(power*costable(res))/64;
					bull->yspeed=(power*sintable(res))/64;
				}

			} while (--rnd>0);


/*			if (power)
				res=AddObject(rx,ry,aPLAYERBULLETP8,PLAYSLOT+nump);
			else
				res=AddObject(rx,ry,aPLAYERBULLET8,PLAYSLOT+nump);
*/
			break;

		case 8:
			if (power)
				res=AddObject(rx,ry,aPLAYERBULLETP9,PLAYSLOT+nump);
			else
				res=AddObject(rx,ry,aPLAYERBULLET9,PLAYSLOT+nump);
			break;

		case 10:

			if (ry>level.waterlevel)
			{
				res=AddObject(rx,ry,aPLAYERBULLETC1,PLAYSLOT+nump);
				if (res>0)
				{
					Omonster *obj=(Omonster *)&gameobj[res];

					obj->curanim=AnimBase[mCOMMON]+mCOMMON_BUBBLES;
					obj->phase=((int)RandFac(255))%3;
					obj->ProcPtr=cBUBBLE;
					obj->nohit=1;
					obj->lighttype=0;
					obj->objtype=oSPECIAL; //nonbullet
				}
			}
			else
				res=AddObject(rx,ry,aPLAYERBULLETC1,PLAYSLOT+nump);
			break;

		case 11:
			res=AddObject(rx,ry,aPLAYERBULLETC2,PLAYSLOT+nump);
			break;

		case 12:
			res=AddObject(rx,ry,aPLAYERBULLETC3,PLAYSLOT+nump);
			break;

		case 13:
			res=AddObject(rx,ry,aPLAYERLASER,PLAYSLOT+nump);
			break;

		default:
			res=0;
			break;
	}; //switch firetype

	if (res>0)
		FixupBullet(nump,res,firetype,uid,0);

	if (sndAudio)	//only for first bullet
		AddBulletSounds(nump,rx,ry);

	return(res);
}	//AddRealBullet




/*
AddPlayBullet()
	main player addbullet routine which triggers new bullets
	at player positions/gun positions.
*/
void AddPlayBullet(int nump,int uid)
{
	Tplayer *play;
	int rx,ry;
//	int res;
	Tframes *frame;
	int power;
	int firetype;

	ASSERT(nump<MAXPLAYERS);
	play=&player[nump];

	if ((play->firetype==0) && (play->shield>0) && (uid==0))
	{
		firetype=9+play->shieldtype;
		power=0;
	}
	else
	{
		firetype=play->firetype;
		power=play->gunpower[play->firetype];
	}


	frame=&frames[play->curframe];


	if (play->firedirection==8)
	{
		if (play->direction<0)
			rx=play->xpos-((frame->tx2-frame->tx3)*65536);
		else
	   		rx=play->xpos+((frame->tx2-frame->tx3)*65536);

		ry=play->ypos-8*65536;//+((frame->ty2-frame->ty3)*65536)+10*65536;
	}
	else
	{
		if (play->direction<0)
			rx=play->xpos;//-((frame->tx2-frame->tx3)*65536)+14*65536;
		else
	   		rx=play->xpos;//+((frame->tx2-frame->tx3)*65536)-14*65536;

		ry=play->ypos+((frame->ty2-frame->ty3)*65536);
	}

	if (play->fly<0)
		ry+=-8*65536+sintable(gamecounter*12)*8;

	rx&=0xffff0000;		//better for multiplayer..... wtfc?
	ry&=0xffff0000;


	AddRealBullet(rx,ry,nump,uid,firetype,power);
}	//AddPlayBullet()



/*
	addbullet
	this should be used for triggering bullets at AI positions
	(when a monster wants to fire a bullet it should use this
	routine)
*/
int AddBullet(int numobj,int numbullet)
{
	Omonster *obj;
	int rx,ry;
	int res;
	Tframes *frame;

	ASSERT(numobj<MAXOBJECTS);
	obj=(Omonster *)&gameobj[numobj];

	frame=&frames[obj->curframe];

	if (obj->direction<0)
		rx=obj->xpos-((frame->tx2-frame->tx3)*65536);
	else
	   	rx=obj->xpos+((frame->tx2-frame->tx3)*65536);

	ry=obj->ypos+((frame->ty2-frame->ty3)*65536);


	res=AddObject(rx,ry,numbullet,numobj);
	if (res>0)
	{
		gameobj[res].direction=obj->direction;
		gameobj[res].xspeed=obj->direction*gameobj[res].xspeed;
		gameobj[res].xacc=obj->direction*gameobj[res].xacc;
	}

	return(res);
} //AddBullet()



/*
	collision with laser beam.
	laser beam cuts horizontally through the level, and hits any
	'active' object at +/-16 pixels height.
	is effective in the 'events triggered range', events which
	are not triggered yet are not affected by this.
*/
int LaserCollision(Tgameobj *obj1,Tgameobj *obj2)
{
	int dy;

	if (obj1->direction<0)
	{
		if (obj2->xpos>obj1->xpos) return(0);

		dy=obj1->ypos-obj2->ypos;
		if (ABS(dy)<16*65536)
		{
			return(1);
		}
	}
	else
	{
		if (obj2->xpos<obj1->xpos) return(0);

		dy=obj1->ypos-obj2->ypos;

		if (ABS(dy)<16*65536)
		{
			return(1);
		}
	}

	return(0);
} //LaserCollision()



/*
	checks bullet collision for a certain bullet

	first checks bullet against all players, then 
	checks bullet against active players,
	then checks bullet against objects
	- when a player is hit (depends on gametype)
	  g-hit.c->HitPlayer is called

    - when an object is hit g_hit.c->HitObject is called

*/
void CheckBullet(int bullet)
{
	Obullets *bull;
	Omonster *mons;
	int num;
	int nump;
	int numhits;
	int gotcha;
	int DoHitPlayer;
	Word8	data[128];
	TactPACKETS *action;


	ASSERT(bullet<MAXOBJECTS);
	bull=(Obullets *)(&gameobj[bullet]);

	action=(TactPACKETS *)(&data[0]);


	if ((MultiPlayerConnection>gameLOCAL) && !LocalComputerIsServer)
	{
		DoHitPlayer=0; 
	}
	else
		DoHitPlayer=1;

/*
	we let the server do all these collisions


	the client also does collision, but doesnt do Hitplayer!

	another approach could be for each client to process it's own LOCAL bullets, and
	send 'hit' information to the server whenever one of its bullets hit a player
	advantage of this might be:
	- lessen workload of server
	- limit amount of packets sent
	- better latency/visual consistency	

	hmmm
*/

	if ((MultiPlayerType!=gameNORMAL) && (MultiPlayerType!=gameCOOPERATIVE))
	for (nump=0;nump<MAXPLAYERS;nump++)
	if ((bull->creator!=PLAYSLOT+nump) && player[nump].Active)
	{
		gotcha=0;	//type of colision for network

		if ((MultiPlayerType==gameBATTLE) || (MultiPlayerType==gameCAPTURE))
		{
			if (player[nump].flicker)
				continue;

			if (bull->creator>=PLAYSLOT) 
			{
				if (player[bull->creator-PLAYSLOT].flicker)	
					continue;

				if (MultiPlayerType==gameCAPTURE)
					if (player[bull->creator-PLAYSLOT].Team==player[nump].Team)
						continue;
			}	
		}	//multipalyer type before collission

		if ((bull->type&bullLASER)==bullLASER)
			numhits=LaserCollision(&gameobj[bullet],(Tgameobj *)(&player[nump]));
		else
			numhits=PixelCollision(&gameobj[bullet],(Tgameobj *)(&player[nump]));

		if (numhits)
		{
			if (bull->creator>=PLAYSLOT)
			{
				if (bull->freeze)
				{
//					if (player[nump].freeze!=0xFF)
					player[nump].freeze=bull->freeze;	//no NRG loss!
					gotcha=actPLAYERFREEZE;
				}
				else
				{
					if (bull->xspeed>0)
						player[nump].xspeed=bull->force*2*65536;
					else
						player[nump].xspeed=-bull->force*2*65536;

					player[nump].yspeed=-(2+bull->force)*4*65536;

					if (player[nump].freeze)
					{
						player[nump].freeze=0;
						cUNFREEZE(PLAYSLOT+nump,1);
					}
					//and! when unfrozen damage is done...............

					if ((MultiPlayerType==gameBATTLE) || 
						(MultiPlayerType==gameCAPTURE) || 
						(MultiPlayerType==gameTREASURE))
					{
						if (DoHitPlayer)
						{
							if (HitPlayer(nump,bullet))
								gotcha=actPLAYERHIT;
						}
					}
					else
					{
						player[nump].bemoved=8+8*bull->force;
						player[nump].idletime=0;
					}
				}
			} else
			{
				if (bull->freeze)
				{
					if (player[nump].freeze!=0xFF)
					{
						player[nump].freeze=bull->freeze;	//no NRG loss!
						gotcha=actPLAYERFREEZE;
					}
				}
				else
				{
					if (DoHitPlayer)
					{
		  				if (HitPlayer(nump,bullet))
							gotcha=actPLAYERHIT;
					}
				}
			}


			if (DoHitPlayer) {
				if ((bull->type&bullLASER)!=bullLASER)
					bull->state=sEXPLODE;
			}

			if ((MultiPlayerConnection>gameLOCAL) && LocalComputerIsServer) {
				if (gotcha==actPLAYERFREEZE) {
					if (bull->state==sEXPLODE) {
						action->GENERIC.PacketID=actPLAYERFREEZEEXPLODE;
						action->PLAYERFREEZEEXPLODE.UIDLow=UID[bullet];
						action->PLAYERFREEZEEXPLODE.UIDHigh=UID[bullet]>>8;
						action->PLAYERFREEZEEXPLODE.nump=nump;
						action->PLAYERFREEZEEXPLODE.freeze=bull->freeze;
					} else {
						action->GENERIC.PacketID=actPLAYERFREEZE;
						action->PLAYERFREEZE.nump=nump;
						action->PLAYERFREEZE.freeze=bull->freeze;
					}
				} else if (gotcha==actPLAYERHIT) {
					action->GENERIC.PacketID=actPLAYERHIT;
					action->PLAYERHIT.UIDLow=UID[bullet];
					action->PLAYERHIT.UIDHigh=UID[bullet]>>8;
					action->PLAYERHIT.nump=nump;
					if (player[nump].energy>0)
						action->PLAYERHIT.energy=player[nump].energy;
					else
						action->PLAYERHIT.energy=0;
					action->PLAYERHIT.explode=(bull->state==sEXPLODE);
					action->PLAYERHIT.unfreeze=(player[nump].freeze==0);
				}

				if (gotcha)
						Server_SendActions(action,0);	//send to all!
			}	//if network

			if (bull->state==sEXPLODE)
				break; //bullet only hits one thing!
		}
	} //for all players







//PROCESS OBJECTS!!!!!!!!!
	for (num=1;num<NumObjects;num++)
	if (gameobj[num].ProcPtr) {
        mons=(Omonster *)&gameobj[num];

    	if ((((mons->objtype&oTYPES)==oNORMAL) || ((mons->objtype&oTYPES)==oSPECIAL)))
    	{
			if ((bull->type&bullLASER)==bullLASER)
				numhits=LaserCollision(&gameobj[bullet],&gameobj[num]);
			else
				numhits=PixelCollision(&gameobj[bullet],&gameobj[num]);

			if (numhits)
			{
				if ((bull->freeze) && ((gameobj[num].nohit&8)!=8))
				{
					if (mons->freeze!=0xFF)
						mons->freeze=bull->freeze;	//no NRG loss!

					if (mons->state!=sFREEZE)
						mons->oldstate=mons->state;
					mons->state=sFREEZE;
					bull->state=sEXPLODE;
				} else {
					HitObject(num,bull->creator,bullet);
				}

				if ((bull->type&bullLASER)!=bullLASER) {
					break; //bullet only hits one thing!
				}
			}
		}
		else
    	if ((mons->objtype&oTYPES)==oPICKUP)
    	{
			if ((bull->type&bullLASER)==bullLASER)
				numhits=LaserCollision(&gameobj[bullet],&gameobj[num]);
			else
				numhits=PixelCollision(&gameobj[bullet],&gameobj[num]);
			if (numhits)
			{
                if ((mons->xspeed==0) && (mons->yspeed==0))
                {
					mons->xspeed=bull->xspeed/2;
					mons->yspeed=bull->yspeed/2;
					mons->direction=bull->direction;
				}
			} //that's it!
    	}
	} //for all active and normal objects
} //CheckBullet()

