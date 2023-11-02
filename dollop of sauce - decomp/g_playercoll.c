/*
	G_PLAYERCOLL.C


	player collision
*/
#include "G_PlayerColl.h"
#include "M_SaveLoad.h"
#include "E_Sound.h"
#include "Random.h"
#include "E_Strings.h"
#include "G_Error.h"
#include "G_Event.h"
#include "E_Loaders.h"
#include "Globals.h"
#include "N_Main.h"
#include "G_Hit.h"
#include "G_PlayerAnim.h"
#include "G_Collision.h"
#include "G_Objects.h"
#include "G_Bosses.h"
#include "G_Ai.h"
#include "E_Particles.h"
#include "G_Stat.h"
#include "G_Bullet.h"
#include "G_AiTypes.h"
#include "G_Find.h"
#include "G_Init.h"

int buttstompsounds[4]=
{	sCOMMON_SPLAT1,sCOMMON_SPLAT2,sCOMMON_SPLAT3,sCOMMON_SPLAT4};

/*
	player2playercol	


  the client does this for his own players only

  when you hit another player
    client-> sent stomp command

    server -> sent hitplayer
*/




// partially decomped
void DoPlayerLandscapeCollision(int nump)
{
	Tplayer *play;
    int px,py;
    int ty;
    int check,calc;
//	int hurt;
    int firstpixely;
    int checkceiling;
    int backtrace;
    int mxspeed,myspeed;
    int newxpos,newypos;
    int stepn,stepx,stepy;
	int checky;
	int oldxspeed,oldyspeed;
	int bx,by;
	int numevent,num;
	int warpid;
	Word8 *DumpBuf;


/*	this function checks the player against a 24x24 pixel block, which
	should be completely empty for the player to walk through.

    totally new method:

    goleft,goright aren't necessarily always updated,
    they only scan in the direction that you're facing+going in.
    that's pretty much like nature behaves, doesn't matter too much

    goup/godown ARE updated (needed for jumping)


*/

	play=&player[nump];


	if ((play->vpole>0) || (play->hpole>0))
	{
		play->xspeed=0; play->xacc=0;
		play->yspeed=0; play->yacc=0;
	}


	oldxspeed=play->xspeed;
	oldyspeed=play->yspeed;

//X SPEED
	play->xspeed+=play->xacc;

	if ((play->xspeed<0) && (!play->goleft))
	{
		play->xspeed=0; play->xacc=0;
	};
	if ((play->xspeed>0) && (!play->goright))
	{
		play->xspeed=0; play->xacc=0;
	};




	if (level.waterlevel<0xfffffff)
	{
		if (play->ypos>12*65536+level.waterlevel)
		{
			if (play->swim==0)
			{
				AddExplosion(play->xpos,level.waterlevel,AnimBase[mCOMMON]+mCOMMON_SPLASH); 
				PlaySample(play->xpos,level.waterlevel,sCOMMON_WATER,0,0);
				play->fly=0;	//to be sure
			}
			play->swim=1;
			if (((gamecounter&63)<31) && (RandFac(7)==0))
				AddObject(play->xpos+play->direction*16*65536,play->ypos-8*65536,aBUBBLE,0);

		}
		else
		if (play->ypos<4*65536+level.waterlevel)
			play->swim=0;


	} else	//to be sure (waterlevel goes away)
		play->swim=0;



	if (play->character==mBIRD)
	{
		if (play->xspeed>4*65536) play->xspeed=4*65536;
		else
		if (play->xspeed<-4*65536) play->xspeed=-4*65536;

		mxspeed=play->xspeed;

		if (play->yspeed>4*65536) play->yspeed=4*65536;
		else
		if (play->yspeed<-4*65536) play->yspeed=-4*65536;

		myspeed=play->yspeed;


	} else
	if (play->character==mFROG)
	{
		if (play->xspeed>4*65536) play->xspeed=4*65536;
		else
		if (play->xspeed<-4*65536) play->xspeed=-4*65536;

		mxspeed=play->xspeed;


		if ((play->godown) && (!play->stop))
		{
			if (play->yspeed<0)
				play->yspeed+=play->yacc+3*8192;	//otherwise jumps to high
			else
				play->yspeed+=play->yacc+8192;//level.gravity;
		}
		else
		if (play->godown || (play->yacc<0))
			play->yspeed+=play->yacc;


		if (play->yspeed<-32*65536) play->yspeed=-32*65536;
		else
		if (play->yspeed>12*65536) play->yspeed=12*65536;

		myspeed=play->yspeed;
		if (myspeed>12*65536) myspeed=12*65536;
		else
		if (myspeed<-8*65536) myspeed=-8*65536;

	}
	else
	{	//!BIRD  and !FROG!!!!!!!!
		if (play->stonedLen)
		{
			if (play->xspeed>2*65536) play->xspeed=2*65536;
			else
			if (play->xspeed<-2*65536) play->xspeed=-2*65536;

			mxspeed=play->xspeed+play->movexspeed;
		}
		else
		if ((play->run) || (play->rundash<0) || play->bemoved || play->specialmove)
		{
			if (!play->bemoved)
			{
				if (play->xspeed>16*65536) play->xspeed=16*65536;
				else
				if (play->xspeed<-16*65536) play->xspeed=-16*65536;
			}

			mxspeed=play->xspeed+play->movexspeed;
			if (!play->bemoved)
			{
				if (mxspeed>8*65536) mxspeed=8*65536;
				else
				if (mxspeed<-8*65536) mxspeed=-8*65536;
			}
		}
		else
		{
			if (play->xspeed>4*65536) play->xspeed=4*65536;
			else
			if (play->xspeed<-4*65536) play->xspeed=-4*65536;

			mxspeed=play->xspeed+play->movexspeed;
		}


		if (play->downattack<DOWNATTACKLEN)
		{
			play->xspeed=0;
			if (play->downattack>=DOWNATTACKWAIT)
			{
				if (play->downattack==DOWNATTACKWAIT)
				{
					if (play->character==mSPAZ)
						PlaySample(play->xpos,play->ypos,sSPAZSOUNDS_YAHOO,0,0);
				}

				play->yspeed=10*65536;
				play->spring=0;
			}
			else
				play->yspeed=4096;
		} else
		if ((play->platform==0) || (play->platformtype==0))
		{
			if (play->helicopter>0)
			{
				if (play->godown)
					play->yspeed+=play->yacc;
			}
			else
			if (!play->hang)
			{

				if ((play->godown) && (!play->stop))
				{
					if (play->ypos>level.waterlevel)
					{
						play->yspeed+=play->yacc+1024;
					}
					else
					{
						if (play->yspeed<0)
							play->yspeed+=play->yacc+3*8192;
						else
							play->yspeed+=play->yacc+8192;//level.gravity;
					}
				}
				else
				if ((play->godown) || (play->yacc<0))
					play->yspeed+=play->yacc;

				if ((play->lastspring+35<(int)gamecounter) && (play->yspeed>32768))
					play->spring=0;
			}
		}

		if (play->yspeed<-32*65536) play->yspeed=-32*65536;
		else
		if (play->yspeed>12*65536) play->yspeed=12*65536;

		myspeed=play->yspeed+play->moveyspeed;
		if (myspeed>12*65536) myspeed=12*65536;
		else
		if (myspeed<-8*65536) myspeed=-8*65536;

	}	//!bird and !frog


//check for conveyor belts
	check=GetEvent((play->xpos+15*65536)/(32*65536),(play->ypos+20*65536)/(32*65536));

	if (!play->godown)
	{
		calc=GetEventParm((play->xpos+15*65536)/(32*65536),(play->ypos+20*65536)/(32*65536),0,-8);
		if (calc==0)
			calc=2*65536;
		else
			calc*=65536;


		if (check==areaBELTRIGHT)
			mxspeed+=calc;
		else
		if (check==areaBELTLEFT)
			mxspeed-=calc;
		else
		if (check==areaBELTACCRIGHT)
		{
			play->xspeed+=calc;
			mxspeed+=calc;
		}
		else
		if (check==areaBELTACCLEFT)
		{
			play->xspeed-=calc;
			mxspeed-=calc;
		}
	}


//check for wind
//need to build in code that finds these events, and then
//executes these parts of code when the events are in a level
//at all! should speed up a tad
	bx=(play->xpos)/(32*65536);
	by=(play->ypos)/(32*65536);
	for (stepy=0;stepy<2;stepy++)
	{
		for (stepx=0;stepx<2;stepx++)	
		{
			check=GetEvent(bx+stepx,by+stepy);
			if (check==areaWINDLEFT)
			{
				//play->xspeed-=16384;
				calc=GetEventParm(bx+stepx,by+stepy,0,-8);
				if (calc==0)
					mxspeed-=2*32768;
				else
					mxspeed+=calc*16384;

				play->idletime=0;
			} else
			if (check==areaWINDRIGHT)
			{
				calc=GetEventParm(bx+stepx,by+stepy,0,-8);
				if (calc==0)
					mxspeed+=2*32768;
				else
					mxspeed+=calc*16384;
				play->idletime=0;
			} else
			if (check==areaFLOATUP)
			{
				//myspeed-=3*65536;
				//play->yspeed=myspeed;
				//play->yacc-=16384;


				play->yspeed-=(8*65536-ABS(play->yspeed));

				//play->yspeed+=(-16*65536-play->yspeed)/16;
				play->yacc=-32768;
				myspeed=play->yspeed;
				if (myspeed>4*65536)
					myspeed=4*65536;

				play->idletime=0;
			}
		}
	}
//end check for wind

	
	newxpos=play->xpos+mxspeed;
	newypos=play->ypos+myspeed;

	CheckDestructScenery(nump,newxpos,newypos);

	if ((NumPlayers>1) || (MultiPlayerConnection>gameLOCAL))
	{
		SolidPlayers(nump,&newxpos,&newypos);
	}	//Multiplayers : solid


	if (play->platformtype==2)	//standing on swinging platform
	{
		play->platform_relx+=play->xspeed;
		newxpos=gameobj[play->platform].xpos+(play->platform_relx&0xffff0000);

		if (play->yspeed>-65536)
			newypos=gameobj[play->platform].ypos+(play->platform_rely&0xffff0000);
	}	//platformtype 2 == another player!

	if (play->ShiftPositionX)
	{
		newxpos=(newxpos/8)*7+(play->ShiftPositionX/8);
		play->ShiftPositionX=0;		
	}

	if ((nowallcollision==1) && (play->fly))
	{
		play->xpos=newxpos;
		play->ypos=newypos;
		play->hang=0;
		play->goleft=1;
		play->goright=1;
		play->godown=1;
		play->goup=1;

		return;
	}

	play->oldyspeed=play->yspeed;
	play->oldxspeed=play->xspeed;
	play->hang=0;
	play->slope=0;


//LIMIT the new positions!

	if (newxpos<play->fixstartx) newxpos=play->fixstartx;
	else
	if (newxpos>level.fixwidth) newxpos=level.fixwidth;

	if (newypos<0) newypos=0;
	else
	if (newypos>level.fixheight) newypos=level.fixheight;


//set top left corner of checking box
    px=(newxpos/65536)-12;
    py=(newypos/65536)-4;

	//first: check if we gonna hit a vine
	//if (myspeed>=0)
	if (play->fly==0)
	{
		for (ty=-8;ty<0;ty++)
		{
			check=TCheckHLine(px,24,py+ty);
			if (check==0) //really nothing there
			{
				check=CheckHLine(px,24,py+ty); //now, test for tileattr3
				if (check && ((tileattr==areaVINE) || (tileattr==areaHOOK)))
				{
					if (play->hang==0)
					{
						if (tileattr==areaVINE)
							play->hang=1;
						else
						{
							play->hang=2;	//HOOK
							//newxpos=((px/32)*32+15)*65536;
							
							play->xspeed=0;
							play->yspeed=0;
						}

						if (play->yspeed>4*65536)
							PlaySample(play->xpos,play->ypos,sCOMMON_FOEW3,0,30000);
					}

					newypos=(9+py+ty)*65536;//+32767;

					//debug6=9+py+ty;
					myspeed=newypos-play->ypos;
					play->yspeed=0;
					play->yacc=0;
					break;
				}
			}
		}	//for ty
	}	//if not a bird/flier: falling


//start checking 24x24 block
	firstpixely=0;

//debug2=0;

    for (ty=1;ty<24;ty++)
    {
        check=TCheckHLine(px,24,py+ty);

        if (check)
        {
			if ((tileattr==areaVINE) || (tileattr==areaHOOK))
			{
				//check=0;
				//break;
			}
			else
			if ((myspeed<0) && (tileattr==areaONEWAY))
        	{
        		//check=0;	//override for ONEWAY+VINE event
				//break;
        	}
			else
        	{
	        	firstpixely=ty;
    	    	break;	//break out of ty loop
    	    }
        }
    }	//for ty



	//debug5=-1;
	//debug4=firstpixely;

	backtrace=0;
    if (firstpixely)	//if there are any pixels inside this block!
    {
		calc=20-ABS(play->xspeed/65536);

	    if (firstpixely>=calc)//4=max stephite
        {
        	checkceiling=0;
			for (ty=1;ty>=firstpixely-24;ty--)
			{
		        check=TCheckHLine(px,24,py+ty);
				if ((myspeed<0) && ((tileattr==areaONEWAY) || (tileattr==areaVINE) || (tileattr==areaHOOK)))
        			check=0;	//override for ONEWAY+VINE event (not bouncing ur head)

		        if (check)
		        {
        			checkceiling=ty;
		        	break;	//break out of ty loop
        		}
			}	//for ty //search ceiling


			//debug6=checkceiling;
			if (checkceiling==0)	//safest!
			{
                play->xpos=newxpos;
                play->ypos=(py+4+(firstpixely-24))*65536+32767;
				play->slope=-1;

				if (play->yspeed>0) play->yspeed=0;
				if (play->yacc>0) play->yacc=0;
				//play->ypos=newypos+(firstpixely-24)*65536;
                backtrace=0;

		        //if (mxspeed<0) play->goleft=1;
		        //else
        		//if (mxspeed>0) play->goright=1;

			} //checkceiling=0
			else
				backtrace=1;

	    } //if this little ledge is below max stephite
	    else
	    	backtrace=1;	//do backtrace

		//debug5=backtrace;
        if (backtrace)
	    {
            //start backtracing, to see how far we could still walk in that
            //particular direction

            stepx=(newxpos-play->xpos)/16;    //8=arbitrary whatever works
            stepy=(newypos-play->ypos)/16;	//do not use mxspeed (clipping!)

            stepn=0;
            do {
			    px=(play->xpos/65536)-12;
			    py=(play->ypos/65536)-4;

			    for (ty=0;ty<24;ty++)	//check 24x24
			    {
			        check=TCheckHLine(px,24,py+ty);
		        	//if ((stepy<0) && ((tileattr==areaONEWAY) || (tileattr==areaVINE)))

					if ((tileattr==areaVINE) || (tileattr==areaHOOK))
        				check=0;	//override for ONEWAY+VINE event

					if (tileattr==areaONEWAY)
					{
						if (myspeed<0) check=0;
						else
						if (ty<20) check=0;
					}

			        if (check) break;
			    } //for ty

			    if (!check)
			    {
				    //if everything OK increase positions
				    play->xpos+=stepx;
				    play->ypos+=stepy;
		            stepn++;
		        }
	        } while ((!check) && (stepn<16));

			{	//if stepped at all!
			    play->xpos-=stepx;
			    play->ypos-=stepy;
			};


        	//OK, xpos/ypos are now the maximum that we can reach
        	//the player doesn't walk hills anymore at these fringes
/*
			if (play->xspeed)
			{
				play->xspeed=0;
				play->xacc=0;
			};

			if (play->yspeed)
			{
				play->yspeed=0; 
				play->yacc=0;
			};
*/
	        //if (mxspeed<0) play->goleft=0;
    	    //else
        	//if (mxspeed>0) play->goright=0;


	    }
    }
	else
	{    //nothing going on, 24x24 block still free
		play->xpos=newxpos;
		play->ypos=newypos;

//TEMP!!!!!!!!!
		if ((play->fly==0) && (ABS(play->xspeed)>65535) && (play->yspeed>0))
		{
		    px=(newxpos/65536)-12;
		    py=(newypos/65536)-4;

			checky=28+(ABS(play->xspeed)/65536);
			if (checky<32) checky=32;

			for (ty=24;ty<checky;ty++)
			{
				check=TCheckHLine(px,24,py+ty);

				if (check)
				{
					play->ypos=(py+4+(ty-24))*65536+32767;
					//debug6=ty-24;
					play->slope=1;
					break;
				}
			}
		}	//play->fly=0

		//play->goleft=1;
		//play->goright=1;

        if (mxspeed<0) play->goleft=1;
        else
        if (mxspeed>0) play->goright=1;
	}


    px=(play->xpos/65536)-12;
    py=(play->ypos/65536)-4;
    check=CheckVLine(px-1,py,20);
    if (check>20) check=0;
    play->goleft=(check==0);

    check=CheckVLine(px+25,py,20);
    if (check>20) check=0;
    play->goright=(check==0);

	if (play->slope==0)
	{
		if ((!play->goright) && (play->xspeed>0))
		{
			play->xspeed=0; play->xacc=0;
		}
		else
		if ((!play->goleft) && (play->xspeed<0))
		{
			play->xspeed=0; play->xacc=0;
		}
	}

//only for trying out stuff, you'll never get there if this flag is 'wrong'
	px=(play->xpos/65536)-12;
	py=(play->ypos/65536)-4;
	play->goup=!CheckHLine(px,24,py-8);
	
	if ((tileattr==areaVINE) || (tileattr==areaHOOK)) play->goup=1;
	else
	if ((play->yspeed<0) && (tileattr==areaONEWAY)) play->goup=1;

	if ((play->slope==0) && (!play->goup) && (play->yspeed<0))
	{
		play->yspeed=0;
	}


	play->godown=!CheckHLine(px,24,py+25);
	if (((tileattr==areaVINE) || (tileattr==areaHOOK)) && (play->hang==0))
		play->godown=1;

	if (backtrace && (play->downattack<DOWNATTACKLEN))
	{
		play->goup=1;
		play->yspeed=-8*32768;
		//ay->yspeed=-abs(play->yspeed)/2;
		play->godown=0;
		play->downattack=DOWNATTACKLEN;	//crash.... if oneway or backtraced
	}

	if ((play->slope==0) && (!play->godown) && (play->yspeed>0))
		play->yspeed=0;

	if (play->godown==0)
	{
		play->gofardown=0;

		if (play->downattack<DOWNATTACKLEN)
		{
			play->downattack=DOWNATTACKLEN;
		} else
		if (play->xspeed==0)
		{
			px=(play->xpos/65536);
		
			check=CheckVLine(px+(play->direction*12),py+32,64);
			if (check>63)
			{
				if (play->ledgewiggle==0)
				{
					play->ledgewiggle=1;
				}
			}
			else
				play->ledgewiggle=0;
		};

	} //if not godown
	else
	{		
		play->gofardown=(CheckVLine(play->xpos/65536,py+32,64)>63);
	};


	if (ABS(oldxspeed-play->xspeed)>4*65536)
	{
		play->QuakeX=ABS(oldxspeed-play->xspeed)/8192;
	};

	if ((!play->godown) && (play->fly==0) && (ABS(oldyspeed-play->yspeed)>3*65536))
	{
		PlaySample(play->xpos,play->ypos,sCOMMON_LANDPOP,0,0);

		if (play->downattack==DOWNATTACKLEN)
		{
//			AddExplosion(play->xpos,play->ypos+20*65536,AnimBase[mAMMO]+mAMMO_BOOM1); 
//			play->QuakeY=5+ABS(oldyspeed-play->yspeed)/65536;	
//			play->Quake=-14;//-play->QuakeY;
			play->Quake=-AISPEED/4;
		}
		else
		if (ABS(oldyspeed-play->yspeed)>5*65536)
			AddExplosion(play->xpos,play->ypos+16*65536,AnimBase[mAMMO]+mAMMO_POOF2); 

	};
//debug2=play->QuakeY;


	if (play->platform && (play->platformtype>0))	//not falling when standing on platform type
	{
		if (play->godown && (play->yspeed>0))
			play->yspeed=0;

		play->godown=0;
		play->gofardown=0;
	};



//CHECK HURT EVENTS:

	px=((play->xpos/65536)-12);
	py=((play->ypos/65536)-4);
	if ((GetEvent(px/32,py/32)==areaHURT) || 
		(GetEvent((px+24)/32,py/32)==areaHURT) || 
		(GetEvent(px/32,py/32+1)==areaHURT) || 
		(GetEvent((px+24)/32,py/32+1)==areaHURT))
	{
	    for (ty=1;ty<32;ty++)
		{
		    check=TCheckHLine(px,24,py+ty);
	
	        if ((check) && (tileattr==areaHURT))
			{
				HitPlayer(nump,0);
				break;
			}
		} //for ty
	}


//CHECK AREA EVENTS:
	px=play->xpos/(32*65536);
	py=play->ypos/(32*65536);
	if ((px<0) || (px>=(int)level.blockwidth) || (py<0) || (py>=(int)level.blockheight))
	{
		return;
	}

	check=GetEvent(px,py);

	switch (check)
	{
	case aTEXT:
		calc=GetEventParm(px,py,0,8);

		check=FindEmptyMessage(nump,6);
		if ((play->Display[check].State==6) &&
			(play->Display[check].Var1==calc))
			break;

		play->Display[check].State=6;
		play->Display[check].Counter=-6*AISPEED;
		play->Display[check].Var1=calc;

		calc=GetEventParm(px,py,8,1);
		if (calc)
			MySetEvent(px,py,0);
		break;

	case aWATERBLOCK:
		if (play->lastevent!=aWATERBLOCK)
		{
			calc=GetEventParm(px,py,0,-8)*65536;
			AddExplosion(play->xpos,(play->ypos&0xffe00000)+calc,
						AnimBase[mCOMMON]+mCOMMON_SPLASH); 

			PlaySample(play->xpos,(play->ypos&0xffe00000)+calc,sCOMMON_WATER,0,0);
		}
		break;

	case aWATERLEVEL:
		check=GetEventParm(px,py,0,8);

		if (check==0)
		{
			level.newwaterlevel=py*32*65536+15*65536;
		} else
		{
			level.newwaterlevel=check*32*65536;
		};

		if (GetEventParm(px,py,8,1))	//INSTANT!
			level.waterlevel=level.newwaterlevel; 

		break;

	case aMORPHFROG:
		if (play->character!=mFROG)
		{
			PlaySample(play->xpos,play->ypos,sFROG_JAZZ2FROG,0,0);

			play->character=mFROG;
			play->frogmorph=20*7;
			play->fly=0;
		}
		break;


	case aSUCKERTUBE:
		
		calc=px+py*((int)level.blockwidth+1); //LAYER_WIDTH[SPRITELAYER]

		check=GetEventParm(px,py,17,3);

		if ((check) && (calc!=play->lastsuckpos))
		{
			play->xspeed=0;
			play->yspeed=0;

			play->xpos=(play->xpos+(px*32+15)*65536)/2;
			play->ypos=(play->ypos+(py*32+15)*65536)/2;
			play->downattack=DOWNATTACKLEN; //turn it off!
			play->specialmove=0;

			if (-calc==play->lastsuckpos)
			{
				if (play->sucked<=0)
				{
					check=GetEventParm(px,py,14,3);
					//if (check)
					//	PlaySample(play->xpos,play->ypos,sSPAZSOUNDS_HAHAHA,0,0);

					play->xspeed=GetEventParm(px,py,0,-7)*65536;
					play->yspeed=GetEventParm(px,py,7,-7)*65536;

					if (play->yspeed<0)
					{
						play->spring=1;
						play->lastspring=gamecounter;
					}
					play->sucked=AISPEED/4;
					//play->rolling=AISPEED/2;
					play->lastsuckpos=calc; //dont retrigger
				};

			} else
			{
				play->lastsuckpos=-calc;
				play->sucked=check*AISPEED/2;
			};

		} else
		{
			check=GetEventParm(px,py,14,3);
			//if (check)
			//	PlaySample(play->xpos,play->ypos,sSPAZSOUNDS_HAHAHA,0,0);

			play->xspeed=GetEventParm(px,py,0,-7)*65536;
			play->yspeed=GetEventParm(px,py,7,-7)*65536;
			play->xacc=0;
			play->yacc=0;
			

			if (play->yspeed)	//align!
				play->xpos=(px*32+15)*65536;
			if (play->xspeed)	//align!
				play->ypos=(py*32+15)*65536;

			if (play->yspeed<0)
			{
				play->spring=1;
				play->lastspring=gamecounter;
			}

			play->downattack=DOWNATTACKLEN; //turn it off!
			play->specialmove=0;

			play->lastsuckpos=calc; //dont trigger this one again

			play->bemoved=AISPEED/4;
			play->sucked=AISPEED/2;
		}
		break;

	case areaLIMITXSCROLL:
		if (play->fixstartx!=px*32*65536)
		{	//only when not done before (when standing still in
			//this event, scrolling already occurs
			play->fixstartx=px*32*65536; //limit scrolling
			play->fixscrollx=play->viewstartx; 
		}
		break;

	case areaHPOLE:
		if (play->fly==0)
		{
			calc=px+py*((int)level.blockwidth+1);

			if ((calc!=play->lastpolepos) || (play->hpole<-5))
			{
				if (play->xspeed>0)
				{
					play->polespeed=play->xspeed+8*65536;
					if (play->polespeed>20*65536)
						play->polespeed=20*65536;
				}
				else
				{
					play->polespeed=play->xspeed-8*65536;
					if (play->polespeed<-20*65536)
						play->polespeed=-20*65536;
				};

				play->xacc=play->polespeed;
				play->xpos=(px*32+15)*65536;
				play->ypos=(py*32+15)*65536;
				play->spring=0;
				play->hpole=70;
				play->vpole=0;
				play->lastpolepos=calc;
				play->downattack=DOWNATTACKLEN; //turn it off!
				play->specialmove=0;
			}
		}
		break;


	case areaVPOLE:
		if (play->fly==0)
		{
			calc=px+py*((int)level.blockwidth+1);

			if ((calc!=play->lastpolepos) || (play->vpole<-5))
			{
				if (play->yspeed>0)
					play->polespeed=play->yspeed+16*65536;
				else
					play->polespeed=play->yspeed-16*65536;

				play->xpos=(px*32+15)*65536;
				play->ypos=(py*32+15)*65536;
				
				play->vpole=70;
				play->hpole=0;
				play->lastpolepos=calc;
				play->downattack=DOWNATTACKLEN; //turn it off!
				play->specialmove=0;
			}
		}
		break;

	case areaTRIGGERZONE:
		calc=(px+py*((int)level.blockwidth+1))&0xffff;
		{
			int triggerid;
			//int duration;
			int onoff;
			int switsj;
			int testcalc=play->lastpolepos&0xffff;
			int testtrig=play->lastpolepos>>16;

			triggerid=GetEventParm(px,py,0,5);

			if ((calc!=testcalc) && 
				(triggerid!=testtrig))
			{
				switsj=GetEventParm(px,py,6,1);

				if (switsj)
				{
					if (TriggerSceneryActive[triggerid])
						TriggerSceneryActive[triggerid]=0;
					else
						TriggerSceneryActive[triggerid]=1;
				}
				else
				{
					onoff=GetEventParm(px,py,5,1);
					TriggerSceneryActive[triggerid]=onoff;
				}

				play->lastpolepos=testcalc+(testtrig<<16);
			}
		}
		break;


	case areaSETLIGHT:
		calc=px+py*((int)level.blockwidth+1);

		if (calc!=play->lastpolepos)  //hacked! no retriggering here!
		{
			check=GetEventParm(px,py,0,8);
			play->AmbientBack=(check*64)/100;

			play->lastpolepos=calc;
		}
		break;

	case areaRESETLIGHT:
		play->AmbientBack=play->AmbientDefault;
		break;

	case areaECHO:
		level.Echo=GetEventParm(px,py,0,8);
		break;

	case areaFLYOFF:
		if (play->fly==-1)
		{	//airboard floating off

			check=AddObject(play->xpos,play->ypos,aBOUNCEONCE,0);
			if (check>0)
			{
				gameobj[check].curanim=AnimBase[mPICKUPS]+mPICKUPS_GETBOARD;

				gameobj[check].ProcPtr=&cAIRBFALL;
				((Omonster *)(&gameobj[check]))->xspeed=play->xspeed;
			}
		}
    	play->fly=0;
		if (play->character==mBIRD)
			ChangeCharacter(nump,play->orgcharacter);
		break;

	case areaEOL:
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

		if ((MultiPlayerConnection==gameLOCAL) && (level.finish==0) && (SecretGameActive<2))
		{
			if (SecretGameActive==1)	//just made one, reload old level
			{
				strcpy(level.loadname,SecretGame_OriginalLevel);
				usrWantLoad=512;
			}
			else
			{
				check=GetEventParm(px,py,0,1);
				if (check) {
					SaveSecretGame();
				} else {
					strcpy(level.loadname, LevHdr.NextLevel);
				}
			}
			level.finish=1;
			level.finishcounter=1;
		}
		break;

	case areaWARPEOL:
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

		if ((MultiPlayerConnection==gameLOCAL) && (level.finish==0) && (SecretGameActive<2))
		{
			if (SecretGameActive==1)
			{
				strcpy(level.loadname,SecretGame_OriginalLevel);
				usrWantLoad=512;
			}
			else
				strcpy(level.loadname, LevHdr.NextLevel);

			level.finish=2;
			level.finishcounter=1;			
		}
		break;

	case areaWARPSECRET:
		if ((level.finish==0) && (SecretGameActive==1))
			Fatal("Secret level cannot warp to another secret level");

		if (SecretGameActive==0)
		{
			check=GetEventParm(px,py,0,10);
			if (play->gem[0]>=check)
			{
				play->gem[0]-=check;
				if (level.finish==0)
				{
					SaveSecretGame();
					level.finishcounter=1;
					level.finish=2;
				}
			}

			check=FindEmptyMessage(nump,4);
			play->Display[check].Var1=0;
			play->Display[check].State=4;
			play->Display[check].Counter=1; //show coin requirements, or countdown
		}
		break;

	case areaENDMORPH:
		if (play->character!=play->orgcharacter)
		{
			play->character=play->orgcharacter;
			play->morph=-AISPEED/2;
		}
		break;

	case areaWARP:
		check=GetEventParm(px,py,17,1);	//only do this area here at COIN ANIMS

		if ((play->warpcounter<=0) && (check==0))
		{
			check=GetEventParm(px,py,8,8);

			if (play->gem[0]>=check)
			{
				if (check)
				{
					play->gem[0]-=check; //add anim or so

					calc=FindEmptyMessage(nump,4);
					play->Display[calc].State=4;	//show coins
					play->Display[calc].Counter=0;
					play->Display[calc].Var1=0;

					level.changecoins++; //got warped!
				}

				warpid=GetEventParm(px,py,0,8);
				play->warparea=1+warpid;

				DumpBuf=Buffer16K;
				numevent=CountEventParm(areaWARPTARGET,warpid,DumpBuf);
				if (numevent==1)
				{
					play->warpxpos=CalcX;
					play->warpypos=CalcY;
				} else
				if (numevent==0)
					Error_Internal("No Warp Target found!");
				else
				{
					numevent=(play->warprandomizer%numevent)*8;
					play->warprandomizer++;
					play->warpxpos=*(Word32 *)(DumpBuf+numevent);
					play->warpypos=*(Word32 *)(DumpBuf+numevent+4);
				};
				play->warpcounter=0;

				play->fly=0; //reset flying, copter 

				if (GetEventParm(px,py,16,1) && (MultiPlayerType==gameRACE))
				{	//increase lapcounter dude!
					play->lapcounter++; 
					play->lastlapfinish=gamecounter; //
				}
			}
			else
			{
				//NOT ENUF COINS!!!
				if (check)
				{
					calc=FindEmptyMessage(nump,4);

					if (play->Display[calc].State!=4)
						PlaySample(play->xpos,play->ypos,sCOMMON_NOCOIN,0,0);

					play->Display[calc].State=4;
					play->Display[calc].Var1=-1;
					play->Display[calc].Counter=0;
					sprintf(play->Display[calc].Text,"x%u",check-play->gem[0]);

				}	//check
			}
		}
		break;

	case areaROCKTRIGGER:
		check=GetEventParm(px,py,0,8);
		for (num=1;num<NumObjects;num++)
		if ((gameobj[num].ProcPtr==cROCK) && (gameobj[num].state==sSLEEP))
		{
			if (((Omonster *)(&gameobj[num]))->var4==check)
			gameobj[num].state=sACTION;
		}
		break;

	case areaBOSSTRIGGER:

		if (play->bossactive==0)
		{
			int musicid=GetEventParm(px,py,0,1);
			char musicname[80];

			if (sndAudio) {
				strcpy(musicname,"9:Music:BOSS1.S3M");
				if (musicid) {
					musicname[12]='2';
				}
				if (DoesFileExist(musicname)) {
					musicname[13]=0;
					ModMusicStop();
					LoadMusicFile(musicname+8);
					ModMusicResume();
				}
			}

			play->bossactive=1;

			if (play->rush>0)
			{
				play->rush=0;	//turn it off... woa
				if (sndAudio) {
					ModMusicResume();
				}
			}
		}
		break;

	default:
		break;
	}	//check

	play->lastevent=check;

//END CHECK AREA EVENTS
} //DoPlayerLandscapeCollision()




void PutPlayerOnGround(int nump)
{
	int px;
	int py;
	Tplayer *play;

	play=&player[nump];

	if (play->godown==0) return;

//pixel coordinates

	px=(play->xpos/65536)-16;
	py=(play->ypos/65536)-12;

	if (	(px<0) || (py<0) ||
		(px>=level.pixelwidth) || (py>=level.pixelheight)) //check for outside level.....
		return;


	px=(play->xpos/65536)-16;
	do{
		play->ypos+=32768;
		py=(play->ypos/65536)-12;

		play->godown=1-CheckHLine(px,31,py+32);

	} while (play->godown==1);

	play->ypos-=32768;

} //PutPlayerOnGround()




void PlayerAlignToGround(int nump)
{
	int px;
	int py;
	Tplayer *play;

	play=&player[nump];

	if (play->godown==1)
	{
		return;
	}

//pixel coordinates


	px=(play->xpos/65536)-16;
	do {
		play->ypos-=32768;
		py=(play->ypos/65536)-12;

		play->godown=1-CheckHLine(px+2,28,py+32);

	} while (play->godown==0);
	play->ypos+=32768;

} //PlayerAlignToGround()




int CheckLedge(int px,int py)
{
	return 0;
/*
	int blockx,fractx;
	int blocky,fracty;
	int tile0,tile1,tile2,tile3;
	int check;

    if ((px<0) ||
        (py<0) ||
        (px>(level.pixelwidth-64)) ||
        (py>(level.pixelheight-64)))
        return 0;

	return 0;


	blockx=px/32; fractx=px&31;
	blocky=py/32; fracty=py&31;

	tile0=GetSpriteLayerTile(blockx,	blocky);
	tile1=GetSpriteLayerTile(blockx+1,	blocky);
	tile2=GetSpriteLayerTile(blockx,	blocky+1);
	tile3=GetSpriteLayerTile(blockx+1,	blocky+1);

	check=CheckHBlock(tile0,tile1,tile2,tile3,fractx,fracty);
    if ((check>1) && (check<8))
    {
		return ((blocky*32*65536)+(65536*(fracty+check-21)));
    } //found a ledge!
    else
    {
        //debug1=((blocky*32*65536)+(65536*(fracty+check-21)))/65536;
        return 0;
    };
*/
} //CheckLedge()






typedef enum {
	colNONE=0,
	colOBJECTUNFREEZE,
	colOBJECTUNFREEZEKILL,
	colOBJECTKILL,
	colOBJECTKILLSTOMP,
	colOBJECTHIT,

	colEXPLODE,
	colPLAYERHIT,
	colPLAYERHITEXPLODE,

	colPICKUP,
	colPICKUPGEM,

	colLAST
}	COLLRESULTS;


/*
	check collision between players and a certain object
	depending on which kind of object we're hitting different actions
	are taken


	player collided with object, process the collision and give stuff to player
*/
int DoObjectCollision(int numobj,int nump,int downattack)
{
	Tplayer *play;
	Omonster *obj;
	int temp;
	int sample=0;
	int explode=0;
	int result=0;


	obj=(Omonster *)(&gameobj[numobj]);
	play=&player[nump];

	if ((obj->objtype&oTYPES)==oNORMAL)
	{	//HURT YOU LIKE A BITCH!

		if (downattack)
		{
			obj->energy-=4;
			
			if (obj->energy<=0)
			{
				obj->energy=0;

				DoPoints(PLAYSLOT+nump,numobj);
				if (obj->freeze>0)
				{
					cUNFREEZE(numobj,1);
					//obj->code=NULL; //don't display other kill anim
					obj->state=sKILL;
					result=colOBJECTUNFREEZEKILL;
				}
				else
				{
					obj->state=sKILL;
					explode=1;
					PlaySample(obj->xpos,obj->ypos,buttstompsounds[RandFac(3)],0,0);

					result=colOBJECTKILLSTOMP;
					//AddExtra(nump,numobj,3);
				}
			} else
			{
				obj->justhit=FLASHTIME;
				if (obj->freeze>0)
				{
					cUNFREEZE(numobj,1);
					result=colOBJECTUNFREEZE;
				}
				else
				{
					PlaySample(obj->xpos,obj->ypos,buttstompsounds[RandFac(3)],0,0);
					result=colOBJECTHIT;
				}
			}

            if (downattack>0)
            {
				play->downattack=DOWNATTACKLEN; //turn it off!
				play->yspeed=-(play->yspeed/2)-8*65536;
				play->yacc=0;
				play->invincibility=-AISPEED;
				play->specialmove=0;
            }
		}
		else
		{
			HitPlayer(nump,numobj);
			result=colPLAYERHIT;
		}

	}
	else
	if ((obj->objtype&oTYPES)==oENEMYBULLET)
	{
		if ((obj->ProcPtr==&cPLASMAWITCH) &&
			(play->hit<=0) && (play->invincibility==0))
		{
			if ((play->character==mJAZZ) ||
				(play->character==mSPAZ))
			{
				play->character=mFROG;
				play->frogmorph=20*7;
				play->fly=0;

				PlaySample(obj->xpos,obj->ypos,sFROG_JAZZ2FROG,0,0);
			}
			result=colEXPLODE;
		}
		else
		{
			HitPlayer(nump,numobj);
			result=colPLAYERHITEXPLODE;
		}
	
		obj->state=sEXPLODE;
	}
	else
	if ((obj->objtype&oTYPES)==oSPECIAL)
	{
		sample=0;
		result=colOBJECTHIT;
		switch (obj->load)
		{
			case aFLAG:
				result=colNONE;	//no action sent
				break;

			case aGUN2AMMO15:
			case aGUN3AMMO15:
			case aGUN4AMMO15:
			case aGUN5AMMO15:
			case aGUN6AMMO15:
				if (downattack)
				{
					if (obj->freeze>0)
						cUNFREEZE(numobj,1);

					{
                        obj->state=sACTION;
						obj->nohit=1;
					}					
					explode=1;
					sample=sCOMMON_GLASS2;
                    downattack++;
					play->invincibility=-AISPEED;
					DoPoints(PLAYSLOT+nump,numobj);
				}
				else
					result=colNONE;	//no action sent
				break;

			case aGUN1POWER:
			case aGUN2POWER:
			case aGUN3POWER:
			case aGUN4POWER:
			case aGUN5POWER:
			case aTNTPOWER:
			case aGUN8POWER:
			case aGUN9POWER:
				if (downattack)
				{
					if (obj->freeze>0)
						cUNFREEZE(numobj,1);
					
					{
						temp=obj->var4;

						if (play->gunpower[temp]==0)
						{
							play->gunpower[temp]=1;
							play->firetype=temp;

							play->ammo[temp]+=25;
							if (MultiPlayerType>gameCOOPERATIVE)
							{
								if (play->ammo[temp]>50)
									play->ammo[temp]=50;
							}
							else
							{
								if (play->ammo[temp]>99)
									play->ammo[temp]=99;
							}

							temp=FindEmptyMessage(nump,1);
							play->Display[temp].State=1;
							EStringGet(play->Display[temp].Text,sizeof(play->Display[temp].Text),strPOWERUP);
							play->Display[temp].Counter=0;
						}
						else
						{
							if (MultiPlayerType>gameCOOPERATIVE)
							{
								play->ammo[temp]+=25;
								if (play->ammo[temp]>50)
									play->ammo[temp]=50;
							}
							else
							{
								play->ammo[temp]+=25;
								if (play->ammo[temp]>99)
									play->ammo[temp]=99;
							}
						}
					}
					explode=1;
					obj->state=sKILL;
					sample=sCOMMON_GLASS2;

                    downattack++;
					play->invincibility=-AISPEED;
					DoPoints(PLAYSLOT+nump,numobj);
				}						
				else
					result=colNONE;	//no action sent
				break;

			case aGUN6POWER:
				if (downattack)
				{
					if (obj->freeze>0)
						cUNFREEZE(numobj,1);
					
					{
						temp=5;//obj->load-aGUN1POWER;

						if (play->gunpower[temp]==0)
						{
							play->gunpower[temp]=1;
							play->firetype=temp;

							play->ammo[temp]+=25*32;
							if (MultiPlayerType>gameCOOPERATIVE)
							{
								if (play->ammo[temp]>50*32)
									play->ammo[temp]=50*32;
							}
							else
							{
								if (play->ammo[temp]>99*32)
									play->ammo[temp]=99*32;
							}

							temp=FindEmptyMessage(nump,1);
							play->Display[temp].State=1;
							EStringGet(play->Display[temp].Text,sizeof(play->Display[temp].Text),strPOWERUP);
							play->Display[temp].Counter=0;
						}
						else
						{
							if (MultiPlayerType>gameCOOPERATIVE)
							{
								play->ammo[temp]+=25*32;
								if (play->ammo[temp]>50*32)
									play->ammo[temp]=50*32;
							}
							else
							{
								play->ammo[temp]+=25*32;
								if (play->ammo[temp]>99*32)
									play->ammo[temp]=99*32;
							}
						}
					}
					obj->state=sKILL;
					explode=1;
					sample=sCOMMON_GLASS2;
                    downattack++;
					play->invincibility=-AISPEED;
					DoPoints(PLAYSLOT+nump,numobj);
				}
				else
					result=colNONE;	//no action sent
				break;
						
			case aREDSPRING:
			case aGREENSPRING:
			case aBLUESPRING:
			case aFROZENGREENSPRING:
				if (play->ypos>obj->ypos+4*65536) {
					result=colNONE;
					break;
				}
				/* FALLTHRU */
			case aHREDSPRING:
			case aHGREENSPRING:
			case aHBLUESPRING:
				result=colNONE;
				if (obj->freeze==0)
				{
					if (obj->var3>=obj->var2)
					{
						int testx,testx2;	

						//changed this: only sets hit flag when the spring WORKS

						testx=(obj->xpos-play->xpos)/65536;
						
						testx2=ABS(play->xspeed/65536)+6;
						if (testx2<16) testx2=16;

						if (ABS(testx)<testx2)
						{
//							if (obj->state!=sSPRING)
//								result=colOBJECTHIT;
//stuff is done with prediction only: saves packets

							obj->state=sSPRING;
							((Omonster *)(&gameobj[numobj]))->special=nump+PLAYSLOT;

							if ((play->curanim==(int)AnimBase[play->character]+mJAZZ_FALLCRSH) && play->fixanim)
								play->fixanim=0; //hitting spring, get rid of fixed fall rebouncing animation

						}
					}
				}
				break;

			case aSWINGVINE:
				if (obj->state==sACTION)
				{
					play->vine=numobj;
				}
				break;

			case aCOLLAPSESCENERY:
				if ((obj->state==sSLEEP) && (play->ypos<obj->ypos))
				{
					if (obj->counter==0)
						obj->counter=1;
				};
				break;

//this stuff has a special detection going on at the checkplayercollision
			case aBOMB:
				obj->state=sEXPLODE;
				break;

			case aFIRESHIELD:
				if (downattack)
				{
					if (MultiPlayerType>gameCOOPERATIVE)
						play->shield=AISPEED*15;
					else
						play->shield=AISPEED*30;
					play->shieldtype=1;
					play->firetype=0;
					explode=2;
					PlaySample(play->xpos,play->ypos,sCOMMON_GLASS2,0,0);
					sample=sCOMMON_SHIELD1;
					obj->energy=0;
					obj->state=sKILL;
				}
				else
					result=colNONE;	//no action sent

				break;

			case aWATERSHIELD:
				if (downattack)
				{
					if (MultiPlayerType>gameCOOPERATIVE)
						play->shield=AISPEED*15;
					else
						play->shield=AISPEED*30;
					play->shieldtype=2;
					play->firetype=0;
					explode=2;
					PlaySample(play->xpos,play->ypos,sCOMMON_GLASS2,0,0);
					sample=sCOMMON_SHIELD1;
					obj->energy=0;
					obj->state=sKILL;
				}
				else
					result=colNONE;	//no action sent
				break;

			case aLIGHTSHIELD:
				if (downattack)
				{
					if (MultiPlayerType>gameCOOPERATIVE)
						play->shield=AISPEED*15;
					else
						play->shield=AISPEED*30;
					play->shieldtype=3;
					play->firetype=0;
					explode=2;
					PlaySample(play->xpos,play->ypos,sCOMMON_GLASS2,0,0);
					sample=sCOMMON_SHIELD1;
					obj->energy=0;
					obj->state=sKILL;
				}
				else
					result=colNONE;	//no action sent
				break;

			case aLASERSHIELD:
				if (downattack)
				{
					if (MultiPlayerType>gameCOOPERATIVE)
					{
						result=colNONE;
						sample=0;
						break;
					}
					else
						play->shield=AISPEED*40;
					play->shieldtype=4;
					sample=sCOMMON_GLASS2;
					play->firetype=0;
					explode=2;
					obj->energy=0;
					obj->state=sKILL;
				}
				else
					result=colNONE;	//no action sent
				break;



			case aSUPERREDGEM:	//super gem
			case aGUNBARREL:	//barrel
			case aGEMBARREL:
			case aCARROTBARREL:
			case a1UPBARREL:
			case aGUNCRATE:	//wooden box
			case aGEMCRATE:
			case aCARROTCRATE:
			case a1UPCRATE:
			case aBOMBCRATE:
			case aSPRINGCRATE:
			case aTRIGGERCRATE:	//metal crate
				if (downattack)
				{
					if (obj->freeze>0)
						cUNFREEZE(numobj,1);

					obj->state=sACTION; //!
					play->yspeed=-(play->yspeed/2)-4*65536;
					play->yacc=0;

					sample=sCOMMON_GLASS2;
					explode=1;
					DoPoints(PLAYSLOT+nump,numobj);
                    downattack++;
                    play->invincibility=-AISPEED;
				}
				else
					result=colNONE;	//no action sent
				break;

			case aMORPHMONITOR:	//jazz<>spaz post	
				if (downattack)
				{
					if (obj->freeze>0)
						cUNFREEZE(numobj,1);

					{
						if (obj->load==aBIRDMORPHMONITOR)
							ChangeCharacter(nump,mBIRD);
						else
						{
            				if (player[nump].character==mJAZZ)
	            				ChangeCharacter(nump,mSPAZ);
							else
								ChangeCharacter(nump,mJAZZ);
						};
						obj->state=sKILL;
						play->yspeed=-(play->yspeed/2)-4*65536;
						play->yacc=0;
						explode=1;
						DoPoints(PLAYSLOT+nump,numobj);
					}
                    downattack++;
                    play->invincibility=-AISPEED;

				}
				else
					result=colNONE;	//no action sent
				break;

			case aSAVEPOST:	//savepoint
				if ((obj->state==sSLEEP) ||
					((MultiPlayerType==gameCOOPERATIVE) && (obj->state==sDONE)))
				{
					obj->state=sACTION;
					((Omonster *)(&gameobj[numobj]))->special=nump+PLAYSLOT;

					DoPoints(nump+PLAYSLOT,numobj);
				}
				break;
			case aCATERPILLAR:	//caterpillar
				play->touchsolidobject=numobj;
				result=colNONE;	//no action sent
				break;

			case aCATSMOKE:
				if (play->stoned < (play->stonedLen-128)) {
					sample=sCATERPIL_RIDOE;
				}
				play->stonedLen=play->stoned+256;
				result=colNONE;	//no action sent
				break;

			case aFENCER:
				if (downattack)
				{
					obj->justhit=FLASHTIME;
					obj->energy-=4;
					if (obj->freeze>0)
						cUNFREEZE(numobj,1);

					if (obj->energy<=0)
					{
						obj->energy=0;
						obj->state=sKILL;
						explode=1;
						DoPoints(PLAYSLOT+nump,numobj);
					}

					downattack++;
                    play->invincibility=-AISPEED;
				} else
				{
					temp=player[nump].energy;
					HitPlayer(nump,numobj);
					if (temp!=player[nump].energy)
						sample=sFENCER_FENCE1;
				};
				break;

			case aTURTLESHELL:
			case aTUBETURTLE:
				if (downattack)
				{
					sample=sCOMMON_SMASH;
					obj->state=sKILL;
					explode=1;
					DoPoints(PLAYSLOT+nump,numobj);

					downattack++;
                    play->invincibility=-AISPEED;
				}
				else
				{
					if (obj->load==aTUBETURTLE)
						HitPlayer(nump,numobj);
				}
				break;



//bosses
			case aQUEENBOSS:
//you never can touch her!
				break;


			case aSONICBOSS:
				if (obj->ProcPtr==&cSONCBOTTOM) 
				{
					if (downattack)
						break;
					else
						HitPlayer(nump,numobj);
				}
			case aBILSYBOSS:
			case aTUFBOSS:
			case aBUBBA:
			case aROCKTURT:
				if (obj->energy>=0)
				{
					if (downattack)
					{
						if (obj->freeze>0)
							cUNFREEZE(numobj,1);

						obj->justhit=FLASHTIME;
						obj->energy-=4;

						if (obj->energy<=0)
						{
							obj->energy=0;
							obj->counter=0;
							obj->state=sDONE;
							explode=1;
							DoPoints(PLAYSLOT+nump,numobj);
						}
						play->invincibility=-AISPEED/2;
						play->specialmove=0;
						downattack++;                    
					}
					else
					{
						if ((obj->load==aBUBBA) && (obj->state==sROTATE))
							HitThrowPlayer(nump,numobj,obj->xspeed,obj->yspeed);
						else
							HitPlayer(nump,numobj);
					}
				}
				break;


			case aDEVILDEVAN:
				if (obj->var1>0)		//energy in dword
				{
					if (downattack)
					{
						obj->justhit=FLASHTIME;
						if (obj->freeze>0)
							cUNFREEZE(numobj,1);

						obj->var1-=8;	//alternative energy!
						obj->energy=obj->var1/4;

						if (obj->energy<=0)
						{
							if (obj->var8==0)
							{
								obj->state=sWAKE;
								obj->phase=0;
								obj->counter=0;
								obj->var8=1;	//second phase
								obj->var1=400;
								obj->energy=obj->var1/4;
								explode=0;
							}
							else
							{
								obj->var1=0;
								obj->energy=0;
								obj->state=sDONE;
								obj->curanim=AnimBase[mDEVILDEVAN]+mDEVILDEVAN_FLY;
								obj->nohit=3;
								explode=1;
								DoPoints(PLAYSLOT+nump,numobj);
							}
						}

						downattack++;
						play->specialmove=0;
						play->invincibility=-AISPEED/2;
					}
					else
						HitPlayer(nump,numobj);
				}	//obj->var1 (still alive)
				break;


			case aROBOT:
				if (obj->energy>=0)
				{
					if (downattack)
					{
						obj->energy-=4;
						obj->justhit=FLASHTIME;
						if (obj->freeze>0)
							cUNFREEZE(numobj,1);

						if (obj->energy<=0)
						{
							obj->energy=0;
							obj->state=sKILL;
							explode=1;
							DoPoints(PLAYSLOT+nump,numobj);
						};

						downattack++;
						play->specialmove=0;
						play->invincibility=-AISPEED/2;
						
					} else
						HitPlayer(nump,numobj);
				}
				break;



			case aUTERUS:
				if (obj->load==aUTERUS)
				{

					if (obj->state!=sEXTRA)
					{
						obj->energy=loadobj[aUTERUS].energy;
						break;
					}
					else
					if ((obj->nohit!=0) || downattack)
						break;

				}	//fall through to normal code
			case aUTERUSEL:
			case aFLOATSUCKER:
			case aFLOATLIZARD:
				//normal behaviour
				if (downattack)
				{
					obj->energy-=4;
					obj->justhit=FLASHTIME;
					if (obj->freeze>0)
						cUNFREEZE(numobj,1);

					if (obj->energy<=0)
					{
						obj->energy=0;
						obj->state=sKILL;
						
						DoPoints(PLAYSLOT+nump,numobj);

						if ((obj->load==aUTERUS) || (obj->load==aUTERUSEL))
							explode=0;
						else
							explode=1;
					}

					downattack++;
					play->specialmove=0;
                    play->invincibility=-AISPEED/2;
				} else
				HitPlayer(nump,numobj); //haha
				break;


			case aNORMTURTLE: 
				if (downattack)
				{
					obj->energy-=4;
					obj->justhit=FLASHTIME;
					if (obj->freeze>0)
						cUNFREEZE(numobj,1);

					if (obj->energy<=0)
					{
						MyDeleteObject(numobj);//obj->code=NULL;
						//obj->energy=0;
						//obj->state=sKILL;
						explode=1;
						DoPoints(PLAYSLOT+nump,numobj);
					}
					downattack++;
                    play->invincibility=-AISPEED;
				} else
				if (obj->nohit==0)
					HitPlayer(nump,numobj);
				break;

			case aSTEAM:
				play->yspeed=-65536;
				play->idletime=0;
				result=colNONE;	//no action sent
				break;

			case aROCK:
				if (obj->state==sFLY)
				{
					int relspeed;
					relspeed=abs(obj->xspeed)+abs(obj->yspeed);
					if (relspeed>2*65536)
						HitPlayer(nump,numobj);
					else
						result=colNONE;
				}
				else
					result=colNONE;	//no action sent
				break;

			case aGEMSTOMP:
				if (downattack)
					obj->state=sKILL;
				else
					result=colNONE;	//no action sent
				break;

			case aPIN500BUMP:
			case aPINCARROTBUMP:
				obj->state=sACTION;
				obj->var1=nump;
				result=colNONE;
				break;

			case aPINLEFTPADDLE:
			case aPINRIGHTPADDLE:
				result=colNONE;
				break;

			case aDOGGYDOGG: //hitting dog
				if (obj->state!=sACTION)
				{
					PlaySample(obj->xpos,obj->ypos,sDOG_AGRESSIV,0,0);
				};
				/*FALL THRU*/
			case aSKELETON:	//hitting skeleton
				if (downattack)
				{
					obj->energy-=4;
					obj->justhit=FLASHTIME;
					if (obj->freeze>0)
						cUNFREEZE(numobj,1);

					if (obj->energy<=0)
					{
						obj->energy=0;
						obj->state=sKILL;
						explode=1;
						DoPoints(PLAYSLOT+nump,numobj);
					}

					downattack++;
                    play->invincibility=-AISPEED;
				} else
					HitPlayer(nump,numobj); //haha

				if ((obj->state!=sFREEZE) && (obj->state!=sKILL))
				{
					if (obj->state!=sACTION)
					{
						obj->state=sACTION;
						obj->counter=gamecounter&7;	//aligning anim
						obj->var2=255;
					}
					obj->var1=nump;
				} //not frozen
				break;

			case aSPIKEPLATFORM:
				if ((play->yspeed<0) && (play->ypos>obj->ypos))
					HitPlayer(nump,numobj);
				else
					result=colNONE;	//no action sent
				break;

			case aSPIKEBOLL:
			case aSPIKEBOLL3D:
				HitPlayer(nump,numobj);
				break;

			case aCOPTER:	//hit it, hang from it!
				if ((obj->var5) && (play->fly>=0) &&
					((play->character==mSPAZ) || (play->character==mJAZZ)))
				{
					play->fly=1+numobj;//get duration from object

					if (play->lastjump==0)
						obj->var6=1;	//first gotta release jump
					else
						obj->var6=0;

					obj->state=sEXTRA;
					obj->var1=PLAYSLOT+nump;
					obj->var5=0; //clear it now!

					netcopter(numobj);
				}
				else
					result=colNONE;
				break;

			case aCHESHIRE2: //hit it, hang from it!
				if (obj->var5)
				{
					play->fly=1+numobj;//get duration from object

					if (play->lastjump==0)
						obj->var6=1;	//first gotta release jump
					else
						obj->var6=0;

					obj->curanim=AnimBase[mCAT2]+mCAT2_IDLE;
					obj->state=sEXTRA;
					obj->var1=PLAYSLOT+nump;
					obj->counter=0;
					obj->var5=0;
				}
				else
					result=colNONE;	//no action sent
				break;
#if _DEBUG
			case aDESTRUCTSCENERYBOMB:
			case aBIRD: //dont shoot!
			case aBIRDCAGE:
			case aENDOFLEVELPOST:	//eol post
			case aBONUSLEVELPOST:
			case aTNT:
			case aGEMRING:
			case aROTSMALLTREE:
			case aDIAMONDUSPOLE:
			case aJUNGLEPOLE:
			case aCARROTUSPOLE:
			case aPSYCHPOLE:
			case aMOTH:
			case aFRUITPLATFORM:
			case aBOLLPLATFORM:
			case aGRASSPLATFORM:
			case aPINKPLATFORM:
			case aSONICPLATFORM:
			case aBRIDGE:
			case aEVA:
			case aBUBBLE:
			case aBUBBLER:
			case aPLAYERBULLET6:
			case aPLAYERBULLETP6:
			case areaWARP:
			case aBIGROCK:	//no destroying!
			case aBIGBOX:	//no destroying!
			case aDESTRUCTSCENERY:
			case aSTOMPSCENERY:
				result=colNONE;
				break;
#endif
			default:
				result=colNONE;
				break;
		}; //obj->load


        if (downattack>1)
        {
			play->downattack=DOWNATTACKLEN; //turn it off!
			play->yspeed=-(play->yspeed/2)-8*65536;
			play->yacc=0;
        }
		else
		if (downattack==-100)
		{	//we were running really fast, lets bounce off!
			play->xacc=0;
			play->xspeed=-play->xspeed/2;
			play->yspeed=-6*65536;
			play->bemoved=AISPEED/4;
		} 

		if (sample)
			PlaySample(obj->xpos,obj->ypos,sample,0,0);
		else
		if (explode)
			PlaySample(obj->xpos,obj->ypos,buttstompsounds[RandFac(3)],0,0);
	}
	else
	if ((obj->objtype&oTYPES)==oPICKUP)
	{
		sample=sCOMMON_PICKUP1;

		result=colPICKUP;
		switch (obj->load)
		{
//			case aGUN1AMMO3:
		case aGUN2AMMO3: 
		case aGUN3AMMO3:
		case aGUN4AMMO3: 
		case aGUN5AMMO3: 
		case aGUN7AMMO3:
		case aGUN8AMMO3:
		case aGUN9AMMO3:
			temp=obj->var4;//obj->load-aGUN2AMMO3+1;
			if ((play->ammo[temp]==0) && 
				!(play->shieldtype && (play->firetype==0)))
				play->firetype=temp;

			
			if (MultiPlayerType>gameCOOPERATIVE)
			{
				if (play->ammo[temp]>=50)
				{
					play->ammo[temp]=50;
					sample=0;
					return(colNONE);
				}
				play->ammo[temp]+=3;
				if (play->ammo[temp]>50)
					play->ammo[temp]=50;
			}
			else
			{
				if (play->ammo[temp]>=99)
				{
					play->ammo[temp]=99;
					sample=0;
					return(colNONE);
				}
				play->ammo[temp]+=3;
				if (play->ammo[temp]>99)
					play->ammo[temp]=99;
			}

			sample=sCOMMON_PICKUPW1;
			break;

		case aGUN6AMMO3:
			temp=5;
			if ((play->ammo[5]==0) && 
				!(play->shieldtype && (play->firetype==0)))
				play->firetype=5;

			
			if (MultiPlayerType>gameCOOPERATIVE)
			{
				if (play->ammo[temp]>=50*32)
				{
					play->ammo[temp]=50*32;
					sample=0;
					return(colNONE);
				}
				play->ammo[5]+=3*32;
				if (play->ammo[5]>50*32)
					play->ammo[5]=50*32;
			}
			else
			{
				if (play->ammo[temp]>=99*32)
				{
					play->ammo[temp]=99*32;
					sample=0;
					return(colNONE);
				}
				play->ammo[5]+=3*32;
				if (play->ammo[5]>99*32)
					play->ammo[5]=99*32;
			}

			sample=sCOMMON_PICKUPW1;
			break;

		case aGUNFASTFIRE:	//ammo powerup
			play->firespeed-=3;

			if (play->firespeed<MAXFIRESPEED)
				play->firespeed=MAXFIRESPEED;
			else
			{
				temp=FindEmptyMessage(nump,1);
				play->Display[temp].State=1;
				EStringGet(play->Display[temp].Text,sizeof(play->Display[temp].Text),strFASTFIRE);
				play->Display[temp].Counter=0;
			}
			sample=sCOMMON_PICKUPW1;
			break;


		case aREDGEMTEMP:
		case aREDGEM: 
		case aGREENGEM: 
		case aBLUEGEM:
		case aPURPLEGEM:
		case aRECTREDGEM: 
		case aRECTGREENGEM: 
		case aRECTBLUEGEM:
			result=colPICKUPGEM;
			if (MultiPlayerType==gameTREASURE)
			{
				if (obj->var1==2)
					play->gem[1]+=5;
				else
				if (obj->var1==3)
					play->gem[1]+=10;
				else
					play->gem[1]+=1;

				temp=FindEmptyMessage(nump,4);
				play->Display[temp].State=4;
				play->Display[temp].Var1=obj->var1;
				play->Display[temp].Xpos=obj->xpos-play->viewstartx;
				play->Display[temp].Ypos=obj->ypos-play->viewstarty;
				play->Display[temp].Counter=0;
			}
			else
			{
				if (obj->var1==2)
					play->gem[1]+=5;
				else
				if (obj->var1==3)
					play->gem[1]+=10;

				play->gem[obj->var1]++;	//for end of level tally

				if ((play->lastgemcounter/100)<(play->gem[1]/100))
				{	//got another 100! EXTRA LIFE!
					//play->lastgemcounter=(calc/100)*100;
					//lets do this in the display routine, so it will not ever take one away

					temp=FindEmptyMessage(nump,3);
					play->Display[temp].State=3;
					play->Display[temp].Var1=10;	//add extra flag!
					play->Display[temp].Counter=128;

				}
				else
				{
					temp=FindEmptyMessage(nump,4);
					play->Display[temp].State=4;
					play->Display[temp].Var1=obj->var1;
					play->Display[temp].Counter=0;
				}

				play->Display[temp].Xpos=obj->xpos-play->viewstartx;
				play->Display[temp].Ypos=obj->ypos-play->viewstarty;
			}
/*					
//					if ((MultiPlayerType!=gameBATTLE) || 
//						((MultiPlayerType==gameBATTLE) && (play->netcollectgem==obj->var1)))

			} else
			{
				obj->xspeed=play->xspeed/4;
				if (obj->xspeed>0)
					obj->direction=1;
				else
					obj->direction=-1;
				obj->yspeed=play->yspeed/4;

				obj->state=sFLOATFALL;
				return;
			};
*/					break;

		case aSILVERCOIN:
			play->gem[0]++;

			temp=FindEmptyMessage(nump,4);
			play->Display[temp].State=4;
			play->Display[temp].Var1=0; //5 types of gems
			play->Display[temp].Xpos=obj->xpos-play->viewstartx;
			play->Display[temp].Ypos=obj->ypos-play->viewstarty;
			play->Display[temp].Counter=0;
			sample=sCOMMON_COIN;
			break;

		case aGOLDCOIN:
			play->gem[0]+=5;

			temp=FindEmptyMessage(nump,4);

			play->Display[temp].Counter=0;
			play->Display[temp].State=4;
			play->Display[temp].Var1=5; //5 types of gems
			play->Display[temp].Xpos=obj->xpos-play->viewstartx;
			play->Display[temp].Ypos=obj->ypos-play->viewstarty;
			sample=sCOMMON_COIN;
			break;

		case aAIRBOARD:
			play->fly=-1;
			break;

		case aCOKE:
		case aPEPSI:
		case aMILK:
			play->food++;
			if (((play->food%100)==0) && !play->bossactive)
			{	
				play->rush=20*AISPEED;
				if (sndAudio) {
					ModMusicPause();
					PlayPrioritySample(sRUSH_RUSH);
				}
			}

			sample=sCOMMON_DRINKSPAZZ1+RandFac(3);
			break;				

		case aAPPLE:	
		case aBANANA:
		case aCHERRY:
		case aORANGE:
		case aPEAR:
		case aPRETZEL:
		case aSTRAWBERRY:
		case aLEMON:
		case aLIME:
		case aTHING:
		case aWMELON:
		case aPEACH:
		case aGRAPES:
		case aLETTUCE:
		case aEGGPLANT:
		case aCUCUMB:
		case aPIE:
		case aCAKE:
		case aDONUT:
		case aCUPCAKE:
		case aCHIPS:
		case aCANDY1:
		case aCHOCBAR:
		case aICECREAM:
		case aBURGER:
		case aPIZZA:
		case aFRIES:
		case aCHICKLEG:
		case aSANDWICH:
		case aTACOBELL:
		case aWEENIE:
		case aHAM:
		case aCHEESE:
			play->food++;
			if (((play->food%100)==0) && !play->bossactive)
			{	
				play->rush=20*AISPEED;

				if (sndAudio) {
					ModMusicPause();
					PlayPrioritySample(sRUSH_RUSH);
				}
				sample=0;
			}
			else
				sample=sCOMMON_EAT1+RandFac(3);
			break;

		case aENERGYUP:
			if (MultiPlayerType==gameCAPTURE)
			{
				if (play->energy<3)
					play->energy++;
				else
				{
					play->energy=3;
					sample=0;
					return(colNONE);
				}
			}
			else
			{
				if (play->energy<5)
					play->energy++;
				else
				{
					play->energy=5;
					sample=0;
					return(colNONE);
				}
			}
			play->invincibility=AISPEED; //short
			sample=sCOMMON_EAT1+RandFac(3);
//				if (play->character==mSPAZ)
//					PlaySample(play->xpos,play->ypos,sCOMMON_HAPPY,0,0);
			break;


		case aFULLENERGY:
			if (MultiPlayerType==gameCAPTURE)
			{
				if (play->energy<3)
					play->energy=3;
				else
				{
					play->energy=3;
					sample=0;
					return(colNONE);
				}

			}
			else
			{
				if (play->energy<5)
					play->energy=5;
				else
				{
					play->energy=5;
					sample=0;
					return(colNONE);
				}
			}

			play->invincibility=AISPEED*5;

			if (play->character==mSPAZ)
				sample=sSPAZSOUNDS_HAPPY;
			else
			if (play->character==mJAZZ)
				sample=sJAZZSOUNDS_JUMMY;
			else
				sample=sCOMMON_EAT1+RandFac(3);
			break;

		case aMAXWEAPON:
			play->berserk=AISPEED*3;
			break;

		case aAUTOFIRE:
			play->autofire=1;
			break;
		case aFASTFEET:
			play->fastfeet=AISPEED*3;
			break;
		case aEXTRALIFE:
			play->lives++;
			sample=sCOMMON_HARP1;
			break;
		case aINVINCIBILITY:
			play->invincibility=15*AISPEED;
			break;
		case aEXTRATIME:
			play->time+=10*AISPEED;
			break;
		case aFREEZER:
			play->freeze=3*AISPEED; //
			break;
		case aFLYCARROT:
            play->fly=1;//!play->fly;
            break;

		case aSTOPWATCH:
			if ((play->shield) && (play->shieldtype))
				play->shield+=AISPEED*10;
			break;

		default:
			return(colNONE);
			break;
		}; //switch obj load num

		obj->phase=0;
		obj->ProcPtr=&cEXPLODE;
		if (sample==sCOMMON_PICKUP1)
			PickupSample(obj->xpos,obj->ypos);
		else
		if (sample)
			PlaySample(obj->xpos,obj->ypos,sample,0,0);

		DoPoints(PLAYSLOT+nump,numobj);
	}

	if (explode)
		AddParticlePixelExplosion(numobj,2);	//downattack explosion


	return(result);
}	//DoObjectCollision()



/*
NET:
	does the actual collision between a player and an object, and
	sends stuff around

	in network games a actionpacket is sent to the server or to the
	clients which lists the result of this action, this
*/
void CheckPlayerObjectCollision(int numobj)
{
	Tplayer *play;
	Omonster *obj;
	int numhits;
	int downattack;
	int nump;
	int result;

/*
	only for localplayers.

	other players do collision and are triggered on server and on their own computers	
*/
	for (nump=0;nump<MAXPLAYERS;nump++)
	if ((player[nump].Active) && (player[nump].localplayer))
	{
		play=&player[nump];

		if ((MultiPlayerType==gameTREASURE) &&	//when shot in treasure mode: no pickups/collision
			play->flicker)
			continue;

		numhits=PixelCollision((Tgameobj *)play,&gameobj[numobj]);
		if (numhits>7)	//???
		{
			obj=(Omonster *)&gameobj[numobj];

			if (obj->energy<0) 
			{
				downattack=0; //not attackable!
			}
			else
			{
				downattack=((play->downattack<DOWNATTACKLEN) && (play->yspeed>6*65536));
				if (play->rush)
					downattack=1;
                else
				{
					if ((play->character==mSPAZ) && (play->specialmove>10))
						downattack=-1;
					else
					if ((play->character==mJAZZ) && (play->specialmove>16))
						downattack=-1;
					else
					if (obj->freeze>0)
					{
						if (ABS(play->xspeed)>12*65536)
							downattack=-101;
					}
				}
			} //downattack checking


			result=DoObjectCollision(numobj,nump,downattack);


			if ((MultiPlayerConnection>gameLOCAL) && (result!=colNONE))
			{
				TactPACKETS *action;
				Word8	data[128];

				action=(TactPACKETS *)&data[0];
				//client: we hit something, send to server for acknowledgement
				//server: broadcast the result to all clients

				switch (result)
				{
				case colOBJECTUNFREEZEKILL:
					action->GENERIC.PacketID=actOBJECTUNFREEZE;
					action->OBJECTUNFREEZE.UIDLow=UID[numobj];
					action->OBJECTUNFREEZE.UIDHigh=UID[numobj]>>8;
					break;

				case colOBJECTUNFREEZE:
					action->GENERIC.PacketID=actOBJECTUNFREEZEKILL;
					action->OBJECTUNFREEZE.UIDLow=UID[numobj];
					action->OBJECTUNFREEZE.UIDHigh=UID[numobj]>>8;
					break;

				case colOBJECTKILL:
					action->GENERIC.PacketID=actOBJECTKILL;
					action->OBJECTKILL.UIDLow=UID[numobj];						
					action->OBJECTKILL.UIDHigh=UID[numobj]>>8;
					break;

				case colOBJECTHIT:
					action->GENERIC.PacketID=actOBJECTCOLLISION;
					action->OBJECTCOLLISION.UIDLow=UID[numobj];
					action->OBJECTCOLLISION.UIDHigh=UID[numobj]>>8;
					action->OBJECTCOLLISION.nump=nump;

					//pack downattack
					if (downattack==-101)
						action->OBJECTCOLLISION.attacktype=3;
					else
					if (downattack==-1)
						action->OBJECTCOLLISION.attacktype=2;
					else
					if (downattack==1)
						action->OBJECTCOLLISION.attacktype=1;
					else
						action->OBJECTCOLLISION.attacktype=0;
					break;

				case colEXPLODE:
					action->GENERIC.PacketID=actOBJECTEXPLODE;
					action->OBJECTEXPLODE.UIDLow=UID[numobj];
					action->OBJECTEXPLODE.UIDHigh=UID[numobj]>>8;
					break;

				case colPLAYERHIT:
				case colPLAYERHITEXPLODE:
					action->GENERIC.PacketID=actPLAYERHIT;
					action->PLAYERHIT.UIDLow=UID[numobj];
					action->PLAYERHIT.UIDHigh=UID[numobj]>>8;
					action->PLAYERHIT.nump=nump;
					if (player[nump].energy>0)
						action->PLAYERHIT.energy=player[nump].energy;
					else
						action->PLAYERHIT.energy=0;

					if (result==colPLAYERHITEXPLODE)
						action->PLAYERHIT.explode=1;//(bull->state==sEXPLODE);
					else
						action->PLAYERHIT.explode=0;//(bull->state==sEXPLODE);
					action->PLAYERHIT.unfreeze=0;//(player[nump].freeze==0);
					break;


				case colPICKUP:
					action->GENERIC.PacketID=actOBJECTPICKUP;
					action->OBJECTPICKUP.UIDLow=UID[numobj];
					action->OBJECTPICKUP.UIDHigh=UID[numobj]>>8;
					action->OBJECTPICKUP.nump=nump;
					break;

				case colPICKUPGEM:
					action->GENERIC.PacketID=actOBJECTPICKUPGEM;
					action->OBJECTPICKUPGEM.UIDLow=UID[numobj];
					action->OBJECTPICKUPGEM.UIDHigh=UID[numobj]>>8;
					action->OBJECTPICKUPGEM.nump=nump;
					action->OBJECTPICKUPGEM.gems=player[nump].gem[1];
					break;

				default:
					action->GENERIC.PacketID=actNONE;
					break;
				};	//switch result

				if (action->GENERIC.PacketID!=actNONE)
				{
					if (!LocalComputerIsServer)
						Client_SendActions(action);
					else
						Server_SendActions(action,0);	//send to all!
				}	//if valid action		

			}	//if multiplayer
		}	//numhits>threshold
	} //for nump
} //CheckPlayerObjectCollision()
