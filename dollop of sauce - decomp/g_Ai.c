/*
	AI.C

	all AI and object handling functions

	Created: 01-08-1995 by Arjan Brussee
*/
#include "G_Ai.h"
#include "Random.h"
#include "G_Event.h"
#include "G_Error.h"
#include "E_Sound.h"
#include "E_Loaders.h"
#include "E_Parallax.h"
#include "E_Graphics.h"
#include "E_Strings.h"
#include "G_Objects.h"
#include "G_AiTypes.h"
#include "G_Collision.h"
#include "G_PlayerAnim.h"
#include "G_Boxes.h"
#include "G_Find.h"
#include "G_Hit.h"
#include "G_Stat.h"
#include "G_Bullet.h"
#include "G_AiSetup.h"
#include "E_Particles.h"
#include "N_Main.h"
#include "Globals.h"
#include "Grade.h"
#include "N_Chat.h"
#include <math.h>

#if _DEBUG
#define FAIL	Fatal
#else
#define FAIL	1 ? (void)0 : Fatal
#endif

Word8 TriggerSceneryActive[32];

/*********************************************
*
* STANDARD STATES
*
**********************************************/

static int ricosamps[4]={sAMMO_BUL1,sAMMO_BULFL1,sAMMO_BULFL2,sAMMO_BULFL3};

static int LIZARDSOUNDS[4]=
{sLIZARD_LIZ1,sLIZARD_LIZ2,sLIZARD_LIZ4,sLIZARD_LIZ6};
void cSLEEP(int num,int thresh)
{
	int dx;
	int dy;
	int distance;	
	Tplayer *play;
	Omonster *obj;

	obj=(Omonster *)(&gameobj[num]);
	play=&player[0];

	dx=(obj->xpos-play->xpos)/65536;
	dy=(obj->ypos-play->ypos)/65536;

	distance=(dx*dx)+(dy*dy);

	if (distance<thresh)
		obj->state=sWAKE;
} //cSLEEP




void cWALKFALL(int num)
{
	int hit;
	int px,py;
	Omonster *obj;

	obj=(Omonster *)(&gameobj[num]);

	if (obj->xspeed==0) //obj stopped, restore original speed
	{
		obj->xspeed=((Omonster *)(&loadobj[obj->load]))->xspeed;
	}

	obj->xpos+=obj->xspeed;
	obj->ypos+=obj->yspeed;

	if (obj->ypos>level.waterlevel)
	{
		obj->xspeed+=obj->xacc/4;
		obj->yspeed+=obj->yacc/4;

		if (obj->xspeed>3*65536) obj->xspeed=3*65536;
		else
		if (obj->xspeed<-3*65536) obj->xspeed=-3*65536;

		if (obj->yspeed>2*65536) obj->yspeed=2*65536;
		else
		if (obj->yspeed<-2*65536) obj->yspeed=-2*65536;
	}
	else
	{
		obj->xspeed+=obj->xacc;
		obj->yspeed+=obj->yacc;

		if (obj->xspeed>6*65536) obj->xspeed=6*65536;
		else
		if (obj->xspeed<-6*65536) obj->xspeed=-6*65536;

		if (obj->yspeed>4*65536) obj->yspeed=4*65536;
		else
		if (obj->yspeed<-4*65536) obj->yspeed=-4*65536;
	}


//CHECK FOR FALLING
	if (obj->xspeed<0)
		px=-frames[obj->curframe].tx2-(frames[obj->curframe].dx/2);
	else
		px=(frames[obj->curframe].dx/2)+frames[obj->curframe].tx2;

	py=frames[obj->curframe].ty2-frames[obj->curframe].ty;

	hit=CheckPixel(obj->xpos+(px*65536),obj->ypos+(py*65536));

	if (hit==0)
	{
		obj->yacc=level.gravity;
	}
	else
	{
		obj->yacc=0;
		obj->yspeed=0;
	}


//CHECKING FOR BOUNCING OF WALLS OR RAVINES

	if (obj->xspeed<0)
		px=-frames[obj->curframe].tx2-frames[obj->curframe].dx;
	else
		px=frames[obj->curframe].dx+frames[obj->curframe].tx2;

	py=frames[obj->curframe].ty2-frames[obj->curframe].ty-16;

	hit=CheckPixel(obj->xpos+(px*65536),obj->ypos+(py<<16));

	if ((hit==1) || (tileattr==areaSTOPENEMY)) //bounce of wall
	{
		if (obj->xspeed>=0)
			px=-frames[obj->curframe].tx2-frames[obj->curframe].dx;
		else
			px=frames[obj->curframe].dx+frames[obj->curframe].tx2;

		hit=CheckPixel(obj->xpos+(px*65536),obj->ypos+(py<<16));

		if (hit==1)//dont do the direction swap
		{
			obj->state=sWAIT;
		}
		else
		{
			obj->xspeed=-obj->xspeed;
			obj->xpos+=obj->xspeed;
		}
	}

	if (obj->xspeed<0)
		obj->direction=-1;
	else
		obj->direction=1;


	if ((gamecounter&7)==0)
		obj->phase++;
	if (obj->phase>=(int)anims[obj->curanim].numframes)
		obj->phase=0;

	obj->curframe=anims[obj->curanim].framelist[obj->phase];

	DrawGameobj(num);

} //cWALKFALL






void cHIT(int num)
{
	Omonster *obj;

	obj=(Omonster *)(&gameobj[num]);

	if (((obj->objtype&oTYPES)==oNORMAL) && (obj->nohit==0))
	{
		obj->energy--;
		if (obj->energy<=0)
		{
			cKILL(num);	//kill it now, PUHLEEZE!
		};	

	}; //only should happen for monsters (for now...)
} //cHIT


void cSTILL(int num)
{
	Omonster *obj;

	obj=(Omonster *)(&gameobj[num]);


	if ((gamecounter&3)==0)
		obj->phase++;
	if (obj->phase>=(int)anims[obj->curanim].numframes)
		obj->phase=0;
	
	obj->curframe=anims[obj->curanim].framelist[obj->phase];

	DrawGameobj(num); //tho never freezed

} //cSTILL




// not proper
void cFLOAT(int num)
{
	Omonster *obj=(Omonster *)(&gameobj[num]);
	int gemAnim = AnimBase[mPICKUPS]+mPICKUPS_GEMSMALL;
	int cryAnim = AnimBase[mPICKUPS]+mPICKUPS_CRYSMALL;
	int fase;

	if ((gamecounter&3)==0)
	{
		obj->phase++;
		if (obj->phase>=(int)anims[obj->curanim].numframes)
			obj->phase=0;
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
	}

	fase=num*8+gamecounter;
	if (obj->ypos>level.waterlevel)
		fase=fase*2+((obj->xpos/65536)+(obj->ypos/65536)*256)*16;
	else
		fase=(fase+(obj->xpos/65536)+(obj->ypos/65536)*256)*16;


	if (obj->justhit)
	{
		obj->justhit--;
		AddSprite(obj->xpos,obj->ypos+(sintable(fase)*4),SPRITELAYER,obj->direction,spriteSINGLECOLOR,15,obj->curframe);
	}
	else if (obj->curanim==gemAnim || obj->curanim==cryAnim)		// Floating gem or crystal
		AddSprite(obj->xpos,obj->ypos+(sintable(fase)*4), SPRITELAYER, obj->direction, spriteDIAMOND, obj->var1 - 1, obj->curframe);
	else
		AddSprite(obj->xpos,obj->ypos+(sintable(fase)*4),SPRITELAYER,obj->direction,spriteNORMAL,0,obj->curframe);
} //cFLOAT






// not proper
static void cFLOATFALL(int num)
{
	Omonster *obj = (Omonster *)(&gameobj[num]);
	int gemAnim = AnimBase[mPICKUPS]+mPICKUPS_GEMSMALL;
	int cryAnim = AnimBase[mPICKUPS]+mPICKUPS_CRYSMALL;
	int hit,px,py;
//	int goup,godown,goleft,goright;
//	int speed;


/*
	goup=CheckPixel(obj->xpos,obj->ypos-5*65536);
	godown=CheckPixel(obj->xpos,obj->ypos+5*65536);
	goleft=CheckPixel(obj->xpos-4*65536,obj->ypos);
	goright=CheckPixel(obj->xpos+4*65536,obj->ypos);

	if ((obj->yspeed<0) && (goup==1))
	{
		obj->yspeed=-obj->yspeed;
	} else
	if ((obj->yspeed>0) && (godown==1))
	{
		obj->yspeed=-obj->yspeed;
	};

	if ((obj->xspeed<0) && (goleft==1))
	{
		obj->xspeed=-obj->xspeed;
	} else
	if ((obj->xspeed>0) && (goright==1))
	{
		obj->xspeed=-obj->xspeed;
	};
*/
	px=(frames[obj->curframe].tx2)*65536;
	py=(2+frames[obj->curframe].ty2+frames[obj->curframe].dy)*65536;

	if (CheckPixel(obj->xpos+obj->xspeed,obj->ypos+py))	//stuck
	{
		obj->xspeed=0;
	}
	else if (obj->xspeed<0)
	{
		if (obj->xspeed<-8192)
			obj->xspeed+=8192;
		else
			obj->xspeed=0;
	}
	else if (obj->xspeed>0)
	{
		if (obj->xspeed>8192)
			obj->xspeed-=8192;
		else
			obj->xspeed=0;
	};

	obj->xpos+=obj->xspeed;
	obj->ypos+=obj->yspeed;
	obj->yspeed+=obj->yacc;

	hit=CheckPixel(obj->xpos+px,obj->ypos+py);

	if (hit==0)
	{
		if (obj->ypos>level.waterlevel)
			obj->yacc=level.gravity/4;
		else
			obj->yacc=level.gravity;
	}
	else
	{
        if ((obj->yspeed>4*65536) && ((obj->load==aSILVERCOIN) || (obj->load==aGOLDCOIN)))
			PlaySample(obj->xpos,obj->ypos,sCOMMON_COIN,0,0);
			//PlaySpecialSample(sPICKUPS_ITEMPU1A,22050*2);//-Rand2Fac(4095));


		obj->yspeed=-(obj->yspeed/2);

		if (ABS(obj->yspeed)<16384)
			obj->yspeed=0;
		else
			obj->ypos+=obj->yspeed;
	};


	if ((gamecounter&3)==0)
	{
		obj->phase++;
		if (obj->phase>=(int)anims[obj->curanim].numframes)
			obj->phase=0;
	}
	obj->curframe=anims[obj->curanim].framelist[obj->phase];

	if (obj->curanim==gemAnim || obj->curanim==cryAnim)
		AddSprite(obj->xpos,obj->ypos+(sintable(num*8*16+gamecounter*16)*4), SPRITELAYER, obj->direction, spriteDIAMOND, obj->var1 - 1, obj->curframe);
	else if (obj->yspeed==0)
		AddSprite(obj->xpos,obj->ypos+(sintable(num*8*16+gamecounter*16)*4),SPRITELAYER,obj->direction,spriteNORMAL, 0, obj->curframe);
		// Floating diamond or gem
	else
		DrawGameobj(num);


} //cFLOATFALL





/*********************************************
* PARTICLE CODE
* (code without a state!)
*********************************************/

static int PINPADDLE[4]={sPINBALL_FLIP1,sPINBALL_FLIP2,sPINBALL_FLIP3,sPINBALL_FLIP4};

void cFLAME(int num)
{
	Omonster *obj;

	obj=(Omonster *)(&gameobj[num]);


	if ((gamecounter&7)==0)
		obj->phase++;

	if ((obj->ypos>level.waterlevel) ||
		(obj->phase>=(int)anims[obj->curanim].numframes))
	{
		MyDeleteObject(num);
		return;
	}

	obj->objtype=oHURT;

	obj->channel=PlayLoopSample(obj->xpos,obj->ypos,sCOMMON_BURNIN,63,0,obj->channel);

	obj->curframe=anims[obj->curanim].framelist[obj->phase];

	DrawGameobj(num);

} //cFLAME



void cBUBBLE(int num)
{
	Omonster *obj;
	int px,py,check,calc,nump;


	obj=(Omonster *)(&gameobj[num]);


	for (nump=0;nump<MAXPLAYERS;nump++)
	if (player[nump].Active)
	{
		px=player[nump].xpos;
		py=player[nump].ypos;

		if (
			(obj->xpos<(px-((int)ScreenWidth<<16))) ||
			(obj->xpos>(px+((int)ScreenWidth<<16))) ||
			(obj->ypos<(py-((int)ScreenHeight<<16))) ||
			(obj->ypos>(py+((int)ScreenHeight<<16))))
			continue;
		else
			goto SKIPOVER;
	};
	MyDeleteObject(num);
SKIPOVER:

	check=CheckPixel(obj->xpos,obj->ypos-4*65536);
	if (check && (obj->counter>AISPEED)) 
		MyDeleteObject(num);
	
	obj->yspeed-=4096;
	if (obj->yspeed<-2*65536) obj->yspeed=-2*65536;

	obj->ypos+=obj->yspeed;
	if (obj->ypos<level.waterlevel)
		MyDeleteObject(num);


	px=obj->xpos/(32*65536);
	py=obj->ypos/(32*65536);

	check=GetEvent(px,py);
	if (check==areaWINDLEFT)
	{
		calc=GetEventParm(px,py,0,-8);
		if (calc==0)
			obj->xspeed-=32768;
		else
			obj->xspeed+=calc*8192;

		if (obj->xspeed>8*65536)
			obj->xspeed=8*65536;
		else
		if (obj->xspeed<-8*65536)
			obj->xspeed=-8*65536;
	} else
		obj->xspeed+=Rand2Fac(8191);

	obj->xpos+=obj->xspeed;

	obj->counter++;
/*
	obj->counter++;
	if (obj->counter>4)
	{
		obj->counter=0;
		obj->phase++;
		if (obj->phase>=anims[obj->curanim].numframes)
			obj->phase=0;
	};
*/
	obj->curframe=anims[obj->curanim].framelist[obj->phase];

	DrawGameobj(num);

} //cBUBBLE()








void cSTEAM(int num)
{
	Omonster *obj;


	obj=(Omonster *)(&gameobj[num]);


	if (obj->state==sSTART)
	{
		obj->state=sFLY; //whatever
		obj->xpos-=8*65536; //align on panflyte
		PutObjectOnGround(num);
		obj->ypos+=2*65536; //align

		obj->counter=((obj->xpos/(32*65536))&3)*32; //for delays and such
		obj->phase=0;
	};

	if (obj->state==sDEACTIVATE)
	{
		cDEACTIVATE(num);
		return;
	};
	

	if (obj->counter>=96)
	{
		if (obj->counter==96)
			PlaySample(obj->xpos,obj->ypos,sSTEAM_STEAM,0,0);
		if ((obj->counter&3)==3)
		{
			obj->phase++;
			if (obj->phase>=(int)anims[obj->curanim].numframes)
			{
				obj->counter=0;
				obj->phase=0;
				return; //don't display
			};
		};		
		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		DrawGameobj(num);

	}; //else geen display

	obj->counter++;
} //cSTEAM




void cEXPLOSION(int num)
{
	Omonster *obj;

	obj=(Omonster *)(&gameobj[num]);

//	if (obj->curanim==AnimBase[mCOMMON]+mCOMMON_SPLASH)
//		obj->ypos=level.waterlevel; 

	obj->counter++;
	if ((obj->counter&3)==0)
	{
		obj->phase++;
		if (obj->phase>=(int)anims[obj->curanim].numframes)
		{
			MyDeleteObject(num);//obj->code=NULL;
			return;
		}
	}

	obj->curframe=anims[obj->curanim].framelist[obj->phase];
	DrawGameobj(num);

} //cEXPLOSION


void cRANDPART(int num)
{
	Omonster *obj;


	obj=(Omonster *)(&gameobj[num]);

	//if (obj->state==sKILL) return;
	if ((obj->state==sDEACTIVATE) || (obj->state==sKILL))
	{
		MyDeleteObject(num);//obj->code=NULL;
		return;
	}

	if (obj->state==sSTART)
	{
		obj->xpos+=Rand2Fac(32767);
		obj->ypos+=-16384-RandFac(32767);

		obj->xspeed=Rand2Fac(32767)*16;
		obj->yspeed=(-16384+RandFac(32767))*16;

		if (obj->ypos>level.waterlevel)
		{
			obj->xspeed/=4;
			obj->yspeed/=4;
		};
		if (obj->xspeed<0)
			obj->direction=-1;

		obj->phase=((int)RandFac(16383))%anims[obj->curanim].numframes;
		obj->state=sACTION;	//whatever
	};

	obj->xpos+=obj->xspeed;
	obj->ypos+=obj->yspeed;

	if (CheckPixel(obj->xpos,obj->ypos+5*65536))
	{
		if (obj->var6)
			PlaySample(obj->xpos,obj->ypos,obj->var6,0,0);

		obj->yspeed=-(obj->yspeed/2);

		if (ABS(obj->yspeed)<4096)
			MyDeleteObject(num);//obj->code=NULL;

	} else
	{
		if (obj->ypos>level.waterlevel)
			obj->yspeed+=level.gravity/4;
		else
			obj->yspeed+=level.gravity;
	};


	if ((gamecounter&3)==0)
	{
		obj->phase++;
		if (obj->phase>=(int)anims[obj->curanim].numframes)
			obj->phase=0;
	};

	if (!InViewArea(obj->xpos,obj->ypos)) 
		MyDeleteObject(num);//obj->code=NULL;


	obj->curframe=anims[obj->curanim].framelist[obj->phase];

	DrawGameobj(num);

} //RANDPART()






void cBONE(int num)
{
	Omonster *obj;


	obj=(Omonster *)(&gameobj[num]);

	//if (obj->state==sKILL) return;
	if ((obj->state==sDEACTIVATE) || (obj->state==sKILL))
	{
		MyDeleteObject(num);//obj->code=NULL;
		return;
	}

	if (obj->state==sSTART)
	{
		obj->xpos+=Rand2Fac(32767);
		obj->ypos+=-16384-RandFac(32767);

		obj->xspeed=Rand2Fac(32767)*16;
		obj->yspeed=(-16384+RandFac(32767))*16;

		if (obj->ypos>level.waterlevel)
		{
			obj->xspeed/=4;
			obj->yspeed/=4;
		};

		if (obj->xspeed<0)
			obj->direction=-1;

		obj->phase=((int)RandFac(16383))%anims[obj->curanim].numframes;
		obj->state=sACTION;	//whatever
	};

	if (obj->state==sFREEZE)
	{
        if (obj->freeze>0) obj->freeze--;
        if (obj->freeze==0) obj->state=obj->oldstate;
	} else
	{
		obj->xpos+=obj->xspeed;
		obj->ypos+=obj->yspeed;

		if (CheckPixel(obj->xpos,obj->ypos+5*65536))
		{
			if (obj->var6)
				PlaySample(obj->xpos,obj->ypos,obj->var6,32+ABS(obj->yspeed/8192),22050-RandFac(4095));

			obj->yspeed=-(obj->yspeed/2);

			if (abs(obj->yspeed)<16384)
				MyDeleteObject(num);//obj->code=NULL;

		} else
		{
			if (obj->ypos>level.waterlevel)
				obj->yspeed+=level.gravity/4;
			else
				obj->yspeed+=level.gravity;
		}
		

		if ((gamecounter&3)==0)
		{
			obj->phase++;
			if (obj->phase>=(int)anims[obj->curanim].numframes)
				obj->phase=0;
		}
	}

	if (!InViewArea(obj->xpos,obj->ypos)) 
		MyDeleteObject(num);//obj->code=NULL;


	obj->curframe=anims[obj->curanim].framelist[obj->phase];

	DrawGameobj(num);

} //cBONE()




void cRANDPART2(int num)
{
	Omonster *obj;


	obj=(Omonster *)(&gameobj[num]);

	if ((obj->state==sDEACTIVATE) || (obj->state==sKILL))
	{
		MyDeleteObject(num);//obj->code=NULL;
		return;
	}

	if (obj->state==sSTART)
	{
		obj->xpos+=Rand2Fac(32767);
		obj->ypos+=-16384-RandFac(32767);

		obj->xspeed=Rand2Fac(32767)*16;
		obj->yspeed=(-16384+RandFac(32767))*16;

		if (obj->ypos>level.waterlevel)
		{
			obj->xspeed/=4;
			obj->yspeed/=4;
		};

		if (obj->xspeed<0)
			obj->direction=-1;

		obj->phase=((int)RandFac(16383))%anims[obj->curanim].numframes;
		obj->state=sACTION;	//whatever
	};

	if (obj->state==sFREEZE)
	{
        if (obj->freeze>0) obj->freeze--;
        if (obj->freeze==0) obj->state=obj->oldstate;
	} else
	{
		obj->xpos+=obj->xspeed;
		obj->ypos+=obj->yspeed;

		if ((obj->counter==0) && (CheckPixel(obj->xpos,obj->ypos+5*65536)))
		{
			obj->yspeed=0;	//not really bouncing
			obj->state=sFLY;
			obj->counter=1;


			if (obj->curanim==(int)AnimBase[mQUEEN]+mQUEEN_PART)
				PlaySample(obj->xpos,obj->ypos,sCOMMON_BASE1,50,0);

		} else
		{
			if (obj->ypos>level.waterlevel)
				obj->yspeed+=level.gravity/4;
			else
				obj->yspeed+=level.gravity;
		};

		if ((gamecounter&3)==0)
		{
			obj->phase++;
			if (obj->phase>=(int)anims[obj->curanim].numframes)
				obj->phase=0;
		};
	}; //!freeze


	if (!InViewArea(obj->xpos,obj->ypos)) 
		MyDeleteObject(num);//obj->code=NULL;


	obj->curframe=anims[obj->curanim].framelist[obj->phase];

	DrawGameobj(num);

} //RANDPART2()



void cAIRBFALL(int num)
{
	Omonster *obj;


	obj=(Omonster *)(&gameobj[num]);

	if ((obj->state==sDEACTIVATE) || (obj->state==sKILL))
	{
		MyDeleteObject(num);//obj->code=NULL;
		return;
	}

	if (obj->state==sSTART)
	{
		if (obj->xspeed<0)
			obj->direction=-1;

		obj->ypos+=20*65536;
		obj->state=sFLY;	//whatever
	};

	obj->xpos+=obj->xspeed;
	obj->ypos+=obj->yspeed;

	if (obj->ypos>level.waterlevel)
	{
		obj->yspeed+=level.gravity/8;
		if (obj->yspeed>1*65536)
			obj->yspeed=1*65536;

	}
	else
	{
		obj->yspeed+=level.gravity/2;
		if (obj->yspeed>4*65536)
			obj->yspeed=4*65536;
	};


	if ((gamecounter&3)==0)
	{
		obj->phase++;
		if (obj->phase>=(int)anims[obj->curanim].numframes)
			obj->phase=0;
	};


	if (!InViewArea(obj->xpos,obj->ypos)) 
		MyDeleteObject(num);//obj->code=NULL;


	obj->curframe=anims[obj->curanim].framelist[obj->phase];

	DrawGameobj(num);

} //AIRBFALL()




void cSMOKEPART(int num)
{
	Omonster *obj;

	obj=(Omonster *)(&gameobj[num]);

	if ((obj->state==sKILL) || (obj->state==sDEACTIVATE))
	{
		MyDeleteObject(num);//obj->code=NULL;
		return;
	};

	
	if (obj->state==sSTART)
	{
		if (obj->direction<0)
			obj->var1=512+RandFac(127);
		else
			obj->var1=RandFac(127);

		obj->var2=RandFac(127);
		obj->phase=obj->var2%anims[obj->curanim].numframes;
		obj->state=sFLY; //whatever
	}

	obj->var2++;
	obj->var1++;

	obj->xspeed=sintable(obj->var1);//obj->xacc;
/*	
	if (obj->xspeed>2*65536) obj->xspeed=2*65536;
	else
	if (obj->xspeed<-2*65536) obj->xspeed=-2*65536;
*/	
	obj->yspeed+=obj->yacc;

	obj->xpos+=obj->xspeed;
	obj->ypos+=obj->yspeed;

	if ((obj->var2&7)==0)
		obj->phase++;
	if (obj->phase>=(int)anims[obj->curanim].numframes)
		obj->phase=0;

	obj->curframe=anims[obj->curanim].framelist[obj->phase];

	DrawGameobj(num);

} //cSMOKEPART




/*********************************************
*
* MONSTER CODE
*

lets start some kind of description here:

sKILL:
object is really killed, delete the code pointer, clear up flags and eventual local data. It will not
return the next time (deleted outta eventmap)

sDEACTIVATE:
object is too far from players, it will be deactivated (without getting killed). It has to write itself
back into the eventmap, so that it's triggered next time around.


**********************************************/

void still(int num)
{
	Omonster *obj;
	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		obj->state=sSTILL;
		break;

	case sSTILL:
		cSTILL(num);
		break;

	case sFREEZE:
        if (obj->freeze>0) obj->freeze--;
        if (obj->freeze==0) obj->state=obj->oldstate;
		DrawGameobj(num);
		break;

	case sKILL:
		cKILL(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;

	default:
		FAIL("still(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //switch

} //still object()


void floating(int num)
{
	Omonster *obj=(Omonster *)(&gameobj[num]);
	int t;


	if ((MultiPlayerType==gameNORMAL) && (level.changecoins))
	{
		if (obj->load==aGOLDCOIN)
		{
			obj->load=aGREENGEM;
			obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_GEMSMALL;
			obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
			obj->var1=2;
			obj->points=500;
		}
		else if (obj->load==aSILVERCOIN)
		{
			obj->load=aREDGEM;
			obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_GEMSMALL;
			obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
			obj->var1=1;
			obj->points=100;
		}
	}

	switch (obj->state) {
	case sSTART:
		if ((obj->xspeed) || (obj->yspeed))
		{
			obj->state=sFLOATFALL;
			if (obj->direction==0)
				obj->direction=1;
			else
			if (obj->xspeed<0)
				obj->direction=-1;
		}
		else
		{
			obj->state=sFLOAT;
			if (obj->direction==0)
				obj->direction=1;
			else
			{
				t=((obj->xpos>>21)&1)^((obj->ypos>>21)&1);
				if (t)
					obj->direction=-1;
			};

		};
		break;

	case sFREEZE:
        if (obj->freeze>0) obj->freeze--;
        if (obj->freeze==0) obj->state=obj->oldstate;
		DrawGameobj(num);
		break;

	case sFLOATFALL:
		cFLOATFALL(num);
		break;

	case sFLOAT:
		cFLOAT(num);
		if ((obj->xspeed) || (obj->yspeed)) obj->state=sFLOATFALL;
		break;

	case sEXPLODE:
	case sKILL:
		cKILL(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;

	default:
		FAIL("floating(): Illegal state: obj:%i, type:%i, state:%i\n",num,obj->load,obj->state);
		break;
	}; //switch


	if (obj->var4)
	{
		if (player[0].gunpower[obj->var4])
		{
			if (obj->var4==1)
				obj->curanim=AnimBase[mAMMO]+mAMMO_GUN2ICOP;
			else
			if (obj->var4==2)
				obj->curanim=AnimBase[mAMMO]+mAMMO_GUN3ICOP;
			else
			if (obj->var4==3)
				obj->curanim=AnimBase[mAMMO]+mAMMO_GUN4ICOP;
			else
			if (obj->var4==4)
				obj->curanim=AnimBase[mAMMO]+mAMMO_GUN5ICOP;
			else
			if (obj->var4==5)
				obj->curanim=AnimBase[mAMMO]+mAMMO_GUN6ICOP;
			else
			if (obj->var4==7)
				obj->curanim=AnimBase[mAMMO]+mAMMO_GUN8ICOP;
			else
			if (obj->var4==8)
				obj->curanim=AnimBase[mAMMO]+mAMMO_GUN9ICOP;

		} else
		{
			if (obj->var4==1)
				obj->curanim=AnimBase[mAMMO]+mAMMO_GUN2ITEM;
			else
			if (obj->var4==2)
				obj->curanim=AnimBase[mAMMO]+mAMMO_GUN3ITEM;
			else
			if (obj->var4==3)
				obj->curanim=AnimBase[mAMMO]+mAMMO_GUN4ITEM;
			else
			if (obj->var4==4)
				obj->curanim=AnimBase[mAMMO]+mAMMO_GUN5ITEM;
			else
			if (obj->var4==5)
				obj->curanim=AnimBase[mAMMO]+mAMMO_GUN6ITEM;
			else
			if (obj->var4==7)
				obj->curanim=AnimBase[mAMMO]+mAMMO_GUN8ITEM;
			else
			if (obj->var4==8)
				obj->curanim=AnimBase[mAMMO]+mAMMO_GUN9ITEM;


		};
	};

	if (obj->objtype==oWAITPICKUP)
	{	//a pickup that first flies away, before you can hit it!
		if (obj->var3>0)
			obj->var3--;
		if (obj->var3<=0) 
			obj->objtype=oPICKUP;
	}

} //floating object()



void cAIRBOARD(int num)
{
	Omonster *obj;

	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		obj->state=sFLOAT;
		obj->direction=1;
		break;

	case sFREEZE:
        if (obj->freeze>0) obj->freeze--;
        if (obj->freeze==0) obj->state=obj->oldstate;
		DrawGameobj(num);
		break;

	case sFLOATFALL:
	case sFLOAT:
		cFLOAT(num);
		break;

	case sKILL:
		cKILL(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;

	default:
		FAIL("cAIRBOARD(): Illegal state: obj:%i, type:%i, state:%i\n",num,obj->load,obj->state);
		break;
	}	//switch
} //cAIRBOARD()



void timedfloating(int num)
{
	Omonster *obj=(Omonster *)(&gameobj[num]);
	int t;

	switch (obj->state)
	{
	case sSTART:
		if ((obj->xspeed) || (obj->yspeed))
			obj->state=sFLOATFALL;
		else
		{
			obj->state=sFLOAT;
			if (obj->direction==0)
				obj->direction=1;
			else
			{
				t=((obj->xpos>>21)&1)^((obj->ypos>>21)&1);
				if (t)
					obj->direction=-1;
			};
		};
		break;

	case sFREEZE:
        if (obj->freeze>0) obj->freeze--;
        if (obj->freeze==0) obj->state=obj->oldstate;
		DrawGameobj(num);
		break;
        //fall thru
	case sFLOATFALL:
		cFLOATFALL(num);
		if (obj->yspeed==0)
			obj->state=sFLOAT;
		break;

	case sFLOAT:
		obj->counter--;
		
		if (obj->counter<(3*AISPEED))
		{
			if (framecounter&1)
				cFLOAT(num);

			if (obj->counter<=0)
			{
				MyDeleteObject(num);//obj->code=NULL;
				return;
			}
		} else
			cFLOAT(num);

		if ((obj->xspeed) || (obj->yspeed)) obj->state=sFLOATFALL;
		break;

	case sKILL:
		cKILL(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;

	default:
		FAIL("floating(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //switch

	if (obj->objtype==oWAITPICKUP)
	{	//a pickup that first flies away, before you can hit it!
		if (obj->var3>0)
			obj->var3--;
		if (obj->var3<=0) 
			obj->objtype=oPICKUP;
	}

} //timed floating object()



void cDiamondsAreForever(int num)
{	//gem that stays always
	Omonster *obj;


	obj=(Omonster *)(&gameobj[num]);

	if (obj->state==sFREEZE)
	{
		obj->state=obj->oldstate;
		obj->freeze=0;
	};


	if (obj->state==sSTART)
	{
		obj->state=sFLOATFALL;
		if (obj->direction==0)
			obj->direction=1;
		else
		if (obj->xspeed<0)
			obj->direction=-1;

		if (obj->xspeed>15*65536)
			obj->xspeed=15*65536;
		else
		if (obj->xspeed<-15*65536)
			obj->xspeed=-15*65536;

		obj->var3=AISPEED/4;
		
	} else
	if (obj->state==sFLOAT)
	{
		cFLOAT(num);
		if (obj->xspeed || obj->yspeed) obj->state=sFLOATFALL;
	} else
		cFLOATFALL(num);

	if (obj->objtype==oWAITPICKUP)
	{	//a pickup that first flies away, before you can hit it!
		if (obj->var3>0)
			obj->var3--;
		if (obj->var3<=0) 
			obj->objtype=oPICKUP;
	}


} //cDiamondsAreForever()



void walker(int num)
{
	Omonster *obj;
	int oldy;
	Word8 oldfreeze;

	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		PutWalkerOnGround(num);
		obj->state=sWALK;
		break;

	case sWAKE:
	case sWALK:
		if (obj->curanim==(int)AnimBase[mLIZARD]+mLIZARD_WALK)
		{
			if (((gamecounter&7)==0) && (obj->phase==0))
			{
				PlaySample(obj->xpos,obj->ypos,LIZARDSOUNDS[RandFac(3)],0,0);
			}
		}
		cWALK(num);
		break;

	case sWAIT:
		if ((gamecounter&7)==0)
			obj->phase++;
		if (obj->phase>=(int)anims[obj->curanim].numframes)
			obj->phase=0;
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;

	case sSLEEP:
		cSLEEP(num,16*16);

		if ((gamecounter&7)==0)
			obj->phase++;
		if (obj->phase>=(int)anims[obj->curanim].numframes)
			obj->phase=0;
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;

	case sFREEZE:
		if (obj->freeze>0) obj->freeze--;
		if (obj->freeze==0) obj->state=obj->oldstate;

		if (obj->freeze<30)
		{
			oldy=obj->ypos;		obj->ypos+=4*Rand2Fac(16383);
			oldfreeze=obj->freeze;	obj->freeze=(Word8)RandFac(1); //let freeze anim flicker

			DrawGameobj(num);
			obj->freeze=oldfreeze;
            obj->ypos=oldy;
		} else
		DrawGameobj(num);	

		break;

	case sKILL:
		cKILL(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;

	default:
		FAIL("walker(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //switch
} //walker()







void spring(int num)
{
	Omonster *obj;
	Omonster *obj2;
	Tplayer *play;
	int left,rite;
	int xpos,ypos;
//	int	xdetect;


	obj=(Omonster *)(&gameobj[num]);

	if (obj->var3<obj->var2)	//delay counter (spring wont work after...)
		obj->var3++;

	switch (obj->state)
	{
	case sSTART:
		if (obj->creator!=LEVELSLOT)
		{
			//nothing.

			obj->var2=obj->var3=0;
			PutObjectOnGround(num);
		}
		else
		{
			xpos=obj->xpos/(32*65536);
			ypos=obj->ypos/(32*65536);

			obj->var1=GetEventParm(xpos,ypos,1,2);

			if ((obj->yspeed) && (!obj->xspeed))
			{
				if (GetEventParm(xpos,ypos,0,1))
				{
					obj->ypos-=4*65536;
					obj->yspeed=-obj->yspeed;

					if (obj->load==aREDSPRING)
						obj->curanim=AnimBase[mSPRING]+mSPRING_FRED;
					else
					if (obj->load==aBLUESPRING)
						obj->curanim=AnimBase[mSPRING]+mSPRING_FBLUE;
					else
	//				if (obj->load==aGREENSPRING)	//also frozen
						obj->curanim=AnimBase[mSPRING]+mSPRING_FGREEN;

				} else
					PutObjectOnGround(num);
			};

			if ((obj->xspeed) && (!obj->yspeed))
			{
				left=CheckPixel(obj->xpos-32*65536,obj->ypos);
				rite=CheckPixel(obj->xpos+32*65536,obj->ypos);

				if (rite && (!left))
				{
					obj->xspeed=-obj->xspeed;
					obj->direction=-1;
				};
			};

			if ((MultiPlayerType==gameBATTLE) || (MultiPlayerType==gameRACE) || (MultiPlayerType==gameTREASURE))
			{	//DELAY TIME in seconds in multiplayer games
				obj->var3=obj->var2=AISPEED*GetEventParm(xpos,ypos,3,4);
			}; //else var2=0, which should do the trick
		};

		obj->var4=obj->xpos;
		obj->var5=obj->ypos;

		obj->state=sSLEEP;
		obj->special=0;
		break;

	case sFREEZE:
		if (obj->freeze>0) obj->freeze--;
		if (obj->freeze==0) obj->state=obj->oldstate;
		DrawGameobj(num);
		break;

	case sSLEEP:
		obj->var3++;
		if (obj->var3<obj->var2)
		{
			if (obj->yspeed<0)
			{
				if (obj->ypos<obj->var5+16*65536)
					obj->ypos+=32768;
			}
			else
			if (obj->yspeed>0)
			{
				if (obj->ypos>obj->var5-16*65536)
					obj->ypos-=32768;
			};

			if (obj->xspeed<0)
			{
				if (obj->xpos<obj->var4+16*65536)
					obj->xpos+=32768;
			}
			else
			if (obj->xspeed>0)
			{
				if (obj->xpos>obj->var4-16*65536)
					obj->xpos-=32768;
			};
		}
		else
		if (obj->creator>=LEVELSLOT)
		{
			if (obj->xpos>obj->var4)
				obj->xpos-=32768;
			else
			if (obj->xpos<obj->var4)
				obj->xpos+=32768;
			else
				obj->xpos=obj->var4;

			if (obj->ypos>obj->var5)
				obj->ypos-=32768;
			else
			if (obj->ypos<obj->var5)
				obj->ypos+=32768;
			else
				obj->ypos=obj->var5;
		};
		obj->curframe=anims[obj->curanim].framelist[0];
		DrawGameobj(num);
		break;

	case sSPRING:
		if ((obj->freeze) || (level.finish))
		{
			if (obj->freeze)
			{
				obj->state=sFREEZE;
				obj->oldstate=sSLEEP;
			}
			else
				obj->state=sSLEEP;
            obj->special=0;	//haha, forget it ! no spring this time!
			obj->curframe=anims[obj->curanim].framelist[0];
			DrawGameobj(num);
			break; //exit!
		};

		if (obj->special>=PLAYSLOT)
		{
			play=&player[obj->special-PLAYSLOT];

			
			
			obj->var3=0;
			play->idletime=0;

			if ((obj->yspeed) || ((obj->var1&2)==0))
			{
				play->yspeed=obj->yspeed;
				if (play->yspeed<=0)
					play->downattack=DOWNATTACKLEN;
				else
				if (play->downattack>DOWNATTACKLEN)
					play->downattack=DOWNATTACKWAIT-1;
			}
			

			if ((obj->xspeed) || ((obj->var1&1)==0))
			{
				play->xspeed=obj->xspeed;
				if (obj->xspeed)
				{
					play->rundash=-90;
					play->xacc=play->xspeed/32;
					play->bemoved=1;
					if (play->xspeed<0)
						play->direction=-1;
					else
						play->direction=1;
				}
			}

			play->specialjump=0;
			if (obj->yspeed)
			{
				if (play->spring!=num+65536)
				{
					if (obj->yspeed>0)
						PlaySample(obj->xpos,obj->ypos,sSPRING_BOING_DOWN,0,0);
					else
					if (obj->xspeed)
						PlaySample(obj->xpos,obj->ypos,sSPRING_SPRING1,0,0);
					else
						PlaySample(obj->xpos,obj->ypos,sCOMMON_SPRING1,0,0);
	
					if (play->character==mSPAZ)
						PlaySample(play->xpos,play->ypos,sSPAZSOUNDS_YAHOO2,0,0);
				}
				play->spring=num+65536;	//which spring launched you..
				play->lastspring=gamecounter;
			} else
				PlaySample(obj->xpos,obj->ypos,sSPRING_SPRING1,0,0);


			obj->special=0;
			obj->phase=0;
		} else
		if (obj->special>0)
		{
			obj2=(Omonster *)(&gameobj[obj->special]);

			obj2->yspeed=obj->yspeed/2;

			obj2->xspeed=obj->xspeed/2;

			if (obj2->xspeed<0)
				obj2->direction=-1;
			else
				obj2->direction=1;

			//PlaySample(obj->xpos,obj->ypos,sCOMMON_SPRING1,0,0);
			if (obj->yspeed>0)
				PlaySample(obj->xpos,obj->ypos,sSPRING_BOING_DOWN,0,0);
			else
				PlaySample(obj->xpos,obj->ypos,sSPRING_SPRING1,0,0);


			obj->special=0;
			obj->phase=0;
		}


		if ((gamecounter&3)==0)
			obj->phase++;

		if (obj->phase>=(int)anims[obj->curanim].numframes)
		{
			obj->state=sSLEEP;
			obj->phase=0;
		}

		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;

	case sKILL:
		cKILL(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;

	default:
		FAIL("spring(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //switch

} //spring





void eolpost(int num)
{
	Omonster *obj;


	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		PutObjectOnGround(num);
		obj->state=sSLEEP;
		obj->var1=GetEventParm(obj->xorg/(32*65536),obj->yorg/(32*65536),0,1); //secret or not?
	
		break;

	case sFREEZE:
		obj->freeze=0;
	case sSLEEP:
	case sACTION:
	case sDONE:
	case sKILL:
		DrawGameobj(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;

	default:
		FAIL("eolpost(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //switch

} //eolpost


void savepost(int num)
{
	Omonster *obj;
	Tplayer *play;
	int t,nump;


	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		PutObjectOnGround(num);
		obj->state=sSLEEP;
		obj->var1=0;
		break;


	case sSLEEP:
		obj->phase=0;
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;

	case sFREEZE:
		obj->freeze=0;
		obj->state=sACTION;
	case sACTION:
		if ((gamecounter&7)==0)
		{
			obj->phase++;


			if (obj->phase==4)
				PlaySample(obj->xpos,obj->ypos,sPICKUPS_STRETCH1A,0,0);
			else
			if (obj->phase==6)
				PlaySample(obj->xpos,obj->ypos,sPICKUPS_BOING_CHECK,0,0);
			else
			if (obj->phase==9)
				PlaySample(obj->xpos,obj->ypos,sCOMMON_IMPACT6,0,0);


			if (obj->phase>=(int)anims[obj->curanim].numframes)
			{
				obj->phase=anims[obj->curanim].numframes-1;
				obj->state=sDONE;
				if (obj->special>=PLAYSLOT)
				{
					nump=0;

					if (MultiPlayerType==gameNORMAL)
						nump=obj->special-PLAYSLOT;


					do {
						play=&player[nump];

						if (play->localplayer && play->Active)
						{
							play->xorg=obj->xpos+((frames[obj->curframe].tx2-frames[obj->curframe].tx3)*65536);
							play->yorg=obj->ypos+((frames[obj->curframe].ty2-frames[obj->curframe].ty3)*65536);

							for (t=0;t<5;t++)
								play->oldgem[t]=play->gem[t];

							for (t=0;t<10;t++)
							{
								play->oldammo[t]=play->ammo[t];
								play->oldgunpower[t]=play->gunpower[t];
							}
						}
						if (MultiPlayerType==gameNORMAL)
							break;

						nump++;
						if (nump>=MAXPLAYERS) break;

					} while (1==1);
				}

				obj->var1=1;
			};
		}; //fall thru

	case sDONE:
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;

	case sKILL:
		cKILL(num);
		break;

	case sDEACTIVATE:
		//cDEACTIVATE(num); //don't do this, keep this state
		if (obj->var1==1)
	        obj->state=sDONE;
	    else
	    	obj->state=sSLEEP;
		break;

	default:
		FAIL("savepost(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //switch

} //savepost


void bonuspost(int num)
{
	Omonster *obj;

	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		PutObjectOnGround(num);
		obj->state=sSLEEP;
		obj->var1=0;
		break;

	case sSLEEP:
		obj->phase=0;
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;

	case sFREEZE:
		obj->freeze=0;
	case sDONE:
		obj->state=sACTION;
	case sACTION:
		obj->curanim=AnimBase[mBONUS]+mBONUS_ACTIVE;
		obj->var1=1;
		if (obj->counter>7)
		{
			obj->phase++;
			obj->counter=0;
			if (obj->phase>=(int)anims[obj->curanim].numframes)
				obj->phase=0;
		}; 
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;

	case sKILL:
		cKILL(num);
		break;

	case sDEACTIVATE:
		//cDEACTIVATE(num); //don't do this, keep this state
		if (obj->var1==1)
	        obj->state=sACTION;
	    else
	    	obj->state=sSLEEP;
		break;

	default:
		FAIL("bonuspost(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //switch

} //bonuspost




static void cFALL(int num)
{
	Omonster *obj;
	int t;
	int hitl,hitr;
	int dy;
	int newxpos,newypos;


	obj=(Omonster *)(&gameobj[num]);


	dy=(frames[obj->curframe].ty2+frames[obj->curframe].dy)*65536;


	newxpos=obj->xpos;
	newypos=obj->ypos;

	t=CheckForSpring(num,obj->xpos,obj->ypos);
	if (t)
	{
		//obj->yspeed=gameobj[t].yspeed;
		//obj->yspeed set by spring itself!
		newypos=obj->ypos+obj->yspeed;
	} else
	{
		if (obj->yspeed<0)
		{
			hitl=CheckPixel(obj->xpos-12*65536,obj->ypos+dy);
			hitr=CheckPixel(obj->xpos+12*65536,obj->ypos+dy);

			if (hitl || hitr)
				obj->state=sACTION;

			if (obj->yspeed<-8*65536) 
				obj->yspeed=-8*65536;
		} else
		{
			hitl=CheckPixel(obj->xpos-12*65536,obj->ypos+dy);
			hitr=CheckPixel(obj->xpos+12*65536,obj->ypos+dy);

			if (hitl && hitr)
			{
				if (obj->yspeed>2*65536)
   					obj->state=sACTION;
			} else
			{
				if (hitl) newxpos=obj->xpos+65536;
				if (hitr) newxpos=obj->xpos-65536;
			}

			if (obj->yspeed>8*65536) 
				obj->yspeed=8*65536;
		}

		if (obj->ypos>level.waterlevel)
			obj->yspeed+=level.gravity/4;
		else
		{	//be sure it moves like a player
			if (obj->yspeed<0)
				obj->yspeed+=3*8192;
			else
				obj->yspeed+=8192;
		}

		
		newypos=obj->ypos+obj->yspeed;
	}


	if ((obj->state==sACTION) && (obj->ProcPtr==&boxx))
	{
		obj->xpos=newxpos;
		obj->ypos=newypos;
		return; //dont process the rest!
	}

	if (MultiPlayerConnection>gameLOCAL)
	{
		obj->xpos=newxpos;
		obj->ypos=newypos;
	}
	else
	{
		CollideSolidBox(num,&newxpos,&newypos);

		obj->xpos=newxpos;
		obj->ypos=newypos;

		t=CheckPlayerSolidBox(num);
		if ((t==-1) || (t==1))			
			obj->xspeed=PushSolidBox(num,t);
		else
			obj->xspeed=0;
	}
	
	DrawGameobj(num);

} //cFALL




static void cBoxxProcess(int num)
{
	Omonster *obj;
	int t;
	int hitl,hitr;
	int dy;
	int newxpos,newypos;


	obj=(Omonster *)(&gameobj[num]);

	newxpos=obj->xpos;
	newypos=obj->ypos;


	dy=(frames[obj->curframe].ty2+frames[obj->curframe].dy)*65536;
	hitl=CheckPixel(obj->xpos-12*65536,obj->ypos+dy);
	hitr=CheckPixel(obj->xpos+12*65536,obj->ypos+dy);

	if (!hitl && !hitr)
	{
		obj->state=sFALL;
		obj->yspeed+=level.gravity;
		newypos=obj->ypos+obj->yspeed;
	}
	

	if (MultiPlayerConnection==gameLOCAL)
	{
		CollideSolidBox(num,&newxpos,&newypos);

		obj->xpos=newxpos;
		obj->ypos=newypos;

		t=CheckPlayerSolidBox(num);
		if ((t==-1) || (t==1))
		{
			obj->xspeed=PushSolidBox(num,t);
			if (obj->state==sFREEZE)
				obj->var6=t;
		}
		else
		{
			if (obj->var6)
				PushSolidBox(num,obj->var6);
			else
				obj->xspeed=0;
		}

		t=CheckForSpring(num,obj->xpos,obj->ypos);
		if (t)
			obj->state=sFALL;
	}

	DrawGameobj(num);

} //cBoxxProcess





void monitor(int num)
{
	Omonster *obj;
	int t;


	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		PutObjectOnGround(num);
		if (obj->direction==0)
			obj->direction=1;
		else
		{
			t=((obj->xpos>>21)&1)^((obj->ypos>>21)&1);
			if (t)
				obj->direction=-1;
		};
		obj->state=sSLEEP;
		break;

	case sFREEZE:
		if (obj->freeze>0) obj->freeze--;
		if (obj->freeze==0) 
		{
			obj->state=obj->oldstate;
			obj->var6=0;
		}
		cBoxxProcess(num);
		break;

	
	case sSLEEP:
		if ((gamecounter&7)==0)
		{
			obj->phase++;
			if (obj->phase>=(int)anims[obj->curanim].numframes)
				obj->phase=0;
			obj->curframe=anims[obj->curanim].framelist[obj->phase];
		}

		cBoxxProcess(num);
		break;

	case sFALL:
		cFALL(num);
		break;

	case sACTION:
	case sDONE:
		DrawGameobj(num);
		ClearSolidBox(num);
		break;

	case sKILL: //lolo
		ClearSolidBox(num);
		if (obj->killanim)
		{
			obj->phase=0;
			obj->freeze=0;
			obj->ProcPtr=&cEXPLODE;	//nasty, dont jump back to the original state anymore!
            //if we leave the code the same, the object can again get hit,
            //which sux. this is the easiest solution.
		} else
			MyDeleteObject(num);//obj->code=NULL;
		break;

	case sDEACTIVATE:
		ClearSolidBox(num);
		cDEACTIVATE(num);
		break;

	default:
		FAIL("monitor(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //switch

} //monitor




int boxxguns[8]={aGUN2AMMO3,aGUN2AMMO3,aGUN3AMMO3,aGUN4AMMO3,
				aGUN5AMMO3,aGUN6AMMO3,0,0};
int boxxgems[8]={aREDGEM,aGREENGEM,aBLUEGEM,aPURPLEGEM,
				aRECTREDGEM,aRECTREDGEM,aRECTGREENGEM,aRECTBLUEGEM};


void boxx(int num)
{
	Omonster *obj;
	Omonster *obj2;
	int t,numgems,cobj;
	int loadobj;
	int red,green,blue,purple;



	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		if ((obj->load==aSPRINGCRATE) && (NumPlayers>1))
		{
			obj->load=aGREENSPRING;
			obj->objtype=oSPECIAL;
			obj->yspeed=-24*65536;
			obj->ProcPtr=&spring;
			obj->nohit=1+16;
			obj->curanim=AnimBase[mSPRING]+mSPRING_GREEN;
			return;
		}
		PutObjectOnGround(num);
		obj->state=sSLEEP;
		break;

	case sFREEZE:
		if (obj->freeze>0) obj->freeze--;
		if (obj->freeze<=0)
		{
			obj->state=obj->oldstate;
			obj->var6=0;
		}
		cBoxxProcess(num);
		break;

	case sSLEEP:
		cBoxxProcess(num);
		break;

	case sFALL:
		cFALL(num);
		break;

	case sACTION:
		numgems=8+RandFac(7);

		if (obj->curanim==(int)AnimBase[mPICKUPS]+mPICKUPS_BARREL1)	//barrel!
		{	
			PlaySample(obj->xpos,obj->ypos,sCOMMON_CANSPS,0,0);
			for (t=0;t<numgems;t++)
			{
				cobj=AddObject(obj->xpos,obj->ypos,aSHARD,0);
				if (cobj>0)
				{
					obj2=(Omonster *)(&gameobj[cobj]);
					obj2->curanim=AnimBase[mPICKUPS]+mPICKUPS_BSHARD1+(Word16)RandFac(3);
					//type:RANDPART, randomize done by AI func
				};
			};
		} else
		{
			PlaySample(obj->xpos,obj->ypos,sCOMMON_WOOD1,0,0);
			for (t=0;t<numgems;t++)
			{
				cobj=AddObject(obj->xpos,obj->ypos,aSHARD,0);
				if (cobj>0)
				{
					obj2=(Omonster *)(&gameobj[cobj]);
					obj2->curanim=AnimBase[mPICKUPS]+mPICKUPS_WOOD1+(Word16)RandFac(1);
					//type:RANDPART, randomize done by AI func
				}
			}
		}

		if (obj->special==-1) //GEM BARREL!
		{

			red=GetEventParm(obj->xorg/(32*65536),obj->yorg/(32*65536),0,4);
			green=GetEventParm(obj->xorg/(32*65536),obj->yorg/(32*65536),4,4);
			blue=GetEventParm(obj->xorg/(32*65536),obj->yorg/(32*65536),8,4);
			purple=GetEventParm(obj->xorg/(32*65536),obj->yorg/(32*65536),12,4);
			
			numgems=red+green+blue+purple;
			if (numgems==0)
				numgems=4+RandFac(3);

			for (t=0;t<numgems;t++)
			{
				if (red)
				{
					red--;
					loadobj=aREDGEM;
				} else
				if (green)
				{
					green--;
					loadobj=aGREENGEM;
				} else
				if (blue)
				{
					blue--;
					loadobj=aBLUEGEM;
				} else
				if (purple)
				{
					loadobj=aPURPLEGEM;
					purple--;
				} else
				{		//random
					loadobj=boxxgems[RandFac(7)];	
				};

/*
	these objects normally stand still. we want em to act as random particles
	now, so that's  why the new speeds are patched in heree
*/
				cobj=AddObject(obj->xpos+Rand2Fac(32767)*8,obj->ypos-RandFac(32767)*8,loadobj,0);
				if (cobj>0)
				{
					obj2=(Omonster *)(&gameobj[cobj]);
					obj2->objtype=oWAITPICKUP;
					obj2->var3=AISPEED/4;
					obj2->xspeed=Rand2Fac(32767)*16;
					obj2->yspeed=Rand2Fac(32767)*8;
					if (obj2->xspeed<0)
						obj2->direction=-1;
				}
			} //for gems

		} else
		if (obj->special==1)
		{
			numgems=4+RandFac(3);
			for (t=0;t<=numgems;t++)
			{
				for (loadobj=1;loadobj<6;loadobj++)
					if (player[0].ammo[loadobj]>0)
						break;
				if (loadobj<6)
				{
					do {
						loadobj=1+RandFac(16383)%5;
					} while (player[0].ammo[loadobj]<=0);
				} else
					loadobj=1;
			
				loadobj=boxxguns[loadobj];
/*
	these objects normally stand still. we want em to act as random particles
	now, so that's  why the new speeds are patched in heree
*/
				cobj=AddObject(obj->xpos+Rand2Fac(32767)*8,obj->ypos-RandFac(32767)*8,loadobj,0);
				if (cobj>0)
				{
					obj2=(Omonster *)(&gameobj[cobj]);
					obj2->objtype=oWAITPICKUP;
					obj2->var3=AISPEED/4;
					obj2->xspeed=Rand2Fac(32767)*16;
					obj2->yspeed=Rand2Fac(32767)*8;
					if (obj2->xspeed<0)
						obj2->direction=-1;
				};
		
			};
		} else
		if (obj->special==2)
			AddObject(obj->xpos+Rand2Fac(32767)*8,obj->ypos-RandFac(32767)*8,aENERGYUP,0);
		else
		if (obj->special==3)
		{
			cobj=AddObject(obj->xpos+Rand2Fac(32767)*8,obj->ypos-RandFac(32767)*8,aEXTRALIFE,0);
			if (cobj>0)	//turn on funky trails
				gameobj[cobj].light=1;
		}
		else
		if (obj->special==5)
			AddObject(obj->xpos,obj->ypos,aBOMB,0);
		else
		if (obj->special==6)
		{
			t=GetEventParm(obj->xorg/(32*65536),obj->yorg/(32*65536),0,5);
			TriggerSceneryActive[t]=1;
		}


		if ((obj->load==aGUNCRATE) ||
			(obj->load==aCARROTCRATE) ||
			(obj->load==a1UPCRATE) ||
			(obj->load==aBOMBCRATE) ||
			(obj->load==aSPRINGCRATE))
		{
			t=GetEventParm(obj->xorg/(32*65536),obj->yorg/(32*65536),0,8);
			if (t)
			{

				numgems=GetEventParm(obj->xorg/(32*65536),obj->yorg/(32*65536),8,4);

				if (GetEventParm(obj->xorg/(32*65536),obj->yorg/(32*65536),12,1))
				{
					if ((RandFac(3)==3) && (level.difficulty==2))
					{
						AddObject(obj->xpos+Rand2Fac(32767)*8,obj->ypos-RandFac(32767)*8,aBUMBEE,0);
					}
				}
				

				if (numgems)
				do {
					AddObject(obj->xpos+Rand2Fac(32767)*8,obj->ypos-RandFac(32767)*8,t,0);
				} while (--numgems>0);

			}
		} //extra event?

		if (obj->load==aSPRINGCRATE) {
			ClearSolidBox(num);
			MyDeleteObject(num);

			num=AddObject(obj->xpos,obj->ypos,aGREENSPRING,0);
			if (num>0)
			{
				obj=(Omonster *)(&gameobj[num]);
				obj->objtype|=oALWAYSACTIVE;	//!
			}
			return;
		}
		else
		{
			obj->state=sKILL;
			obj->objtype=oSPECIALDONE;	//don't process this in bullet loop

			DrawGameobj(num);
		}
		ClearSolidBox(num);
		break;

	case sKILL:
		cKILL(num);
		ClearSolidBox(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		ClearSolidBox(num);
		break;

	default:
		FAIL("boxx(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	} //switch

} //boxx


void biggem(int num)
{
	Omonster *obj=(Omonster *)(&gameobj[num]);
	Omonster *obj2;
	int t,numgems,res,random;
	Word8 data[128];
	TactPACKETS *action;
	int startuid=0;

	

	switch (obj->state)
	{
	case sSTART:
		// PutObjectOnGround(num);
		obj->state=sSLEEP;
		break;

	case sSLEEP:
		if ((gamecounter&7)==0)
		{
			obj->phase++;
			if (obj->phase>=(int)anims[obj->curanim].numframes)
				obj->phase=0;
			obj->curframe=anims[obj->curanim].framelist[obj->phase];
		}
		DrawGameobj(num);
		break;

	case sFREEZE:
		if (obj->freeze>0) obj->freeze--;
		if (obj->freeze==0) obj->state=obj->oldstate;
		DrawGameobj(num);
		break;

	case sFALL:
		cFALL(num);
		break;

	case sACTION:
        
		if ((LocalComputerIsServer && (MultiPlayerConnection>gameLOCAL)) || 
			(MultiPlayerConnection==gameLOCAL)) {
			PlaySample(obj->xpos,obj->ypos,sCOMMON_GEMSMSH1,0,0);

			numgems=5+RandFac(7);

			if (MultiPlayerConnection!=gameLOCAL) {
				int Temp;
				action=(TactPACKETS *)(&data[0]);

				action->GENERIC.PacketID=actOBJECTADDTREASUREDIAMONDS;								
				action->OBJECTADDTREASUREDIAMONDS.UIDLow=UID[num];
				action->OBJECTADDTREASUREDIAMONDS.UIDHigh=UID[num]>>8;

				random=RandFac(2047);
				action->OBJECTADDTREASUREDIAMONDS.randomLow3 = random;
				action->OBJECTADDTREASUREDIAMONDS.randomHigh = random>>3;
				action->OBJECTADDTREASUREDIAMONDS.NumGems=numgems;
				action->OBJECTADDTREASUREDIAMONDS.nump=obj->var1;
				startuid=player[obj->var1].CreateUIDCounter&1023;
				action->OBJECTADDTREASUREDIAMONDS.UIDcounterLow=startuid;
				action->OBJECTADDTREASUREDIAMONDS.UIDcounterHigh=startuid>>8;
				Temp = obj->xpos/65536;
				action->OBJECTADDTREASUREDIAMONDS.xposLow=Temp;
				action->OBJECTADDTREASUREDIAMONDS.xposHigh=Temp>>8;
				Temp = obj->ypos/65536;
				action->OBJECTADDTREASUREDIAMONDS.yposLow=Temp;
				action->OBJECTADDTREASUREDIAMONDS.yposHigh=Temp>>8;

				Server_SendActions(action,0);
				RandInit(random);
			}	//!local!
		
			for (t=0;t<numgems;t++)
			{
				res=AddObject(obj->xpos+Rand2Fac(32767)*16,obj->ypos-16*RandFac(32767),aREDGEMTEMP,0);
				if (res>0)
				{
					obj2=(Omonster *)(&gameobj[res]);

					obj2->xspeed=Rand2Fac(32767)*32;
					obj2->yspeed=-(int)RandFac(32767)*16;

					if (MultiPlayerType==gameTREASURE)
					{
						obj2->objtype=oWAITPICKUP; //wait a bit
						obj2->ProcPtr=&cDiamondsAreForever;
						obj2->var3=AISPEED/4;
					}
					else
					{
						obj2->var3=AISPEED/4;
						obj2->objtype=oWAITPICKUP;
					}

					UID[res]=32768+(obj->var1*1024)+(startuid&1023);
					startuid++;
				}
			}

			if (MultiPlayerType>gameLOCAL)
			{
				obj->objtype=oSPECIALDONE;	//don't process this in bullet loop
				obj->state=sDONE;
			}
			else
			{
				MyDeleteObject(num);//obj->code=NULL;
				obj->objtype=oSPECIALDONE;	//don't process this in bullet loop
			}
		}
		break;

	case sDONE:
		obj->objtype=oSPECIALDONE;	//don't process this in bullet loop	//nuttin
		break;

	case sKILL:
		cKILL(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;

	default:
		FAIL("biggem(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	} //switch

} //biggem


void ammomonitor(int num)
{
	Omonster *obj;
	Omonster *obj2;
	int t,cobj;



	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		PutObjectOnGround(num);
		obj->state=sSLEEP;
		break;

	case sFREEZE:
		if (obj->freeze>0) obj->freeze--;
		DrawGameobj(num);

		if (MultiPlayerConnection==gameLOCAL)
		{
			t=CheckPlayerSolidBox(num);
			if ((t==-1) || (t==1))
			{
				obj->var6=t;
			}

			if (obj->var6) //keep on gliding!
				obj->xspeed=PushSolidBox(num,obj->var6);
			else
				obj->xspeed=0;
		}

		if (obj->freeze==0) 
		{
			obj->state=obj->oldstate;
			obj->var6=0;
		}
		break;

	case sSLEEP:
		DrawGameobj(num);

		if (MultiPlayerConnection==gameLOCAL)
		{
			t=CheckPlayerSolidBox(num);
			if ((t==-1) || (t==1))			
				obj->xspeed=PushSolidBox(num,t);
			else
				obj->xspeed=0;
		}
		break;

	case sFALL:
		cFALL(num);
		//ClearSolidBox(num);
		break;

	case sACTION:
		if (obj->var3)
		for (t=0;t<5;t++)
		{
/*	these objects normally stand still. we want em to act as random particles
	now, so that's  why the new speeds are patched in heree
*/
			cobj=AddObject(obj->xpos+Rand2Fac(32767)*8,obj->ypos-RandFac(32767)*8,obj->var3,0);
			if (cobj>0)
			{
				obj2=(Omonster *)(&gameobj[cobj]);
				obj2->objtype=oWAITPICKUP;
				obj2->var3=AISPEED/4;
				obj2->xspeed=Rand2Fac(32767)*16;
				obj2->yspeed=Rand2Fac(32767)*8;
				if (obj2->xspeed<0)
					obj2->direction=-1;
			}
		}

		for (t=0;t<8;t++)
		{
			cobj=AddObject(obj->xpos,obj->ypos,aSHARD,0);
			if (cobj>0)
			{
				obj2=(Omonster *)(&gameobj[cobj]);
				obj2->curanim=AnimBase[mAMMO]+mAMMO_WOODPART1+(Word16)RandFac(1);
				//type:RANDPART, randomize done by AI func
			};
		};

		obj->state=sKILL;
		obj->objtype=oSPECIALDONE;	//don't process this in bullet loop
//		DrawGameobj(num);
		ClearSolidBox(num);
		break;

	case sKILL:
		cKILL(num);
		ClearSolidBox(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		ClearSolidBox(num);
		break;

	default:
		FAIL("ammomonitor(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	} //switch

} //ammomonitor





void cBIGOBJECT(int num)
{
	Omonster *obj;
	int t;



	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		PutObjectOnGround(num);
		obj->state=sSLEEP;
		break;

	case sFREEZE:
		if ((gamecounter&3)==0)
		{
			if (obj->freeze>0) obj->freeze--;
		} //4x slower freezin'!


		if (MultiPlayerConnection==gameLOCAL)
		{
					
			t=CheckPlayerSolidBox(num);
			if ((t==-1) || (t==1))
			{
				obj->var6=t;
			}

			if (obj->var6) //keep on gliding!
				obj->xspeed=PushSolidBox(num,obj->var6);
			else
				obj->xspeed=0;
		}

		if (obj->freeze==0) 
		{
			obj->state=obj->oldstate;
			obj->var6=0;
		}

		DrawGameobj(num);
		break;

	case sACTION:
	case sSLEEP:
		//obj->counter++;
		
		if (MultiPlayerConnection==gameLOCAL)
		{
			t=CheckPlayerSolidBox(num);
			if ((t==-1) || (t==1))			
				obj->xspeed=PushSolidBox(num,t);
			else
				obj->xspeed=0;
		}

		DrawGameobj(num);
		break;

	case sFALL:
		cFALL(num);
		//ClearSolidBox(num);
		break;

	case sKILL:
		cKILL(num);
		ClearSolidBox(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		ClearSolidBox(num);
		break;

	default:
		FAIL("cBIGOBJECT(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //switch

} //cBIGOBJECT






/*******************PSYCH1.LEV********************/



void cheshire2(int num)
{
	Omonster *obj;
	int calc;

	obj=(Omonster *)(&gameobj[num]);

	if ((obj->var1>=PLAYSLOT) && (obj->state!=sEXTRA))
	{	//clear it!!!
		player[obj->var1-PLAYSLOT].fly=0;
		obj->var1=0;
	}


	switch (obj->state)
	{
	case sSTART:
		if (obj->creator!=LEVELSLOT)
		{
			obj->var4=4*AISPEED;
		}
		else
		{	//duration of flying when helicopter is seperate!
			obj->var4=AISPEED*GetEventParm(obj->xpos/(32*65536),obj->ypos/(32*65536),0,8);
			if (obj->var4==0)
				obj->var4=4*AISPEED;
		}

		obj->var5=1;	//not yet hanged on
		obj->state=sFLY;
		obj->curanim=AnimBase[mCAT2]+mCAT2_INVISIBL;
		obj->ypos-=32*65536;
		break; 

	case sSLEEP:	//stay hidden
		obj->counter++;
		if ((obj->counter&7)==0)
		{
			calc=GetNearestPlayer(num,128*64);
			if (calc>=0)
			{
				obj->phase=0;
				obj->curanim=AnimBase[mCAT2]+mCAT2_FADEIN;
				obj->state=sFADEIN;
			}
			else
				obj->phase++;
		}
		if (obj->phase>=(int)anims[obj->curanim].numframes)
			obj->phase=0;

		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;

    case sFADEIN:
		obj->counter++;
    	if ((obj->counter&3)==0) obj->phase++;
		if (obj->phase>=(int)anims[obj->curanim].numframes)
		{
			obj->phase=0;
			obj->state=sFLY;
			obj->curanim=AnimBase[mCAT2]+mCAT2_IDLE;
			obj->counter=0;
		}
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;

	case sFLY:
		obj->counter++;
		if ((obj->counter&7)==0)
		{
			calc=GetNearestPlayer(num,128*64);
			if (calc<0)	//no player near
			{
				obj->state=sFADEOUT;
				obj->curanim=AnimBase[mCAT2]+mCAT2_FADEOUT;
				obj->phase=0;
				obj->counter=0;
			}		
			else
				obj->phase++;
		}
		
		if (obj->phase>=(int)anims[obj->curanim].numframes)
			obj->phase=0;

		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;

    case sFADEOUT:
    	obj->counter++;
		if ((obj->counter&3)==0) obj->phase++;
		if (obj->phase>(int)anims[obj->curanim].numframes-1)
		{
			obj->phase=0;//anims[obj->curanim].numframes-1;
			obj->curanim=AnimBase[mCAT2]+mCAT2_INVISIBL;
			obj->state=sSLEEP;
			obj->counter=0;
		}

		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;

	case sEXTRA:		
		if (obj->var1<PLAYSLOT) {
			obj->state=sDONE;
			DrawGameobj(num);
			return;
		}

		obj->yspeed-=1024;
		obj->ypos+=obj->yspeed;

		obj->counter++;

		if ((obj->var6) && (player[obj->var1-PLAYSLOT].lastjump>0))
			obj->var6--;

		if ((obj->counter>obj->var4) ||
			((obj->var6==0) && (player[obj->var1-PLAYSLOT].lastjump==0)))
		{
			player[obj->var1-PLAYSLOT].fly=0;
			obj->var1=0;
			obj->state=sDONE;
			DrawGameobj(num);
			return;
		}

//		obj->xpos=player[obj->var1-PLAYSLOT].xpos;
		//obj->ypos=player[obj->var1-PLAYSLOT].ypos;//-16*65536;
		obj->phase=0; //dont let him move (looks like shit)
		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		if (obj->counter>obj->var4-AISPEED)
		{
			if (framecounter&1)
				DrawGameobj(num);
		} else
			DrawGameobj(num);
		break;

	case sDONE:
		obj->counter++;

		if (obj->state==sDONE)
			obj->yspeed-=1024;
		obj->ypos+=obj->yspeed;
		if (obj->counter>AISPEED*4) {
			obj->state=sKILL;
		}

		if ((obj->counter&7)==0) obj->phase++;
		if (obj->phase>=(int)anims[obj->curanim].numframes)
			obj->phase=0;
		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		if (obj->counter>AISPEED*3)
		{
			if (framecounter&1)
				DrawGameobj(num);
		} else
			DrawGameobj(num);
		break;

	case sFREEZE:
		if (obj->freeze>0) obj->freeze--;
		if (obj->freeze==0) obj->state=obj->oldstate;
		DrawGameobj(num);
		break;

	case sKILL:
		cKILL(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;

	default:
		FAIL("cheshire2(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}	//obj->state
} //cheshire2()


/////////////HELL.LEV

void skeleton(int num)
{
	Omonster *obj;
	Omonster *obj2;
	int t;
	int oldy;
	Word8 oldfreeze;
	int numstuff;
	int cobj;


	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		PutWalkerOnGround(num);
		obj->state=sWALK;
		break;

	case sACTION:
	case sWALK:
		cWALK(num);
		break;

	case sWAIT:
		if ((gamecounter&7)==0)
			obj->phase++;
		if (obj->phase>=(int)anims[obj->curanim].numframes)
			obj->phase=0;
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;

	case sFREEZE:
		if (obj->freeze>0) obj->freeze--;

		if (obj->freeze<30)
		{
			oldy=obj->ypos;		obj->ypos+=4*Rand2Fac(16383);
			oldfreeze=obj->freeze;	obj->freeze=(Word8)RandFac(1); //let freeze anim flicker

			DrawGameobj(num);
			obj->freeze=oldfreeze;
            obj->ypos=oldy;
			if (obj->freeze==0) 
			{
				cUNFREEZE(num,2);
				obj->state=obj->oldstate;
			};
		} else
		DrawGameobj(num);
		break;

	case sKILL:
		numstuff=32+RandFac(15);
		for (t=0;t<numstuff;t++)
		{
			cobj=AddObject(obj->xpos,obj->ypos-RandFac(32767)*32,aSHARD,0);
			if (cobj>0)
			{
				obj2=(Omonster *)(&gameobj[cobj]);
				if (t==0)
					obj2->curanim=AnimBase[mSKELETON]+mSKELETON_SKULL;
				else
					obj2->curanim=AnimBase[mSKELETON]+mSKELETON_BONEZ;
			
				obj2->ProcPtr=&cBONE;
				switch ((RandFac(3))&7)
				{
					case 0:
						obj2->var6=sSKELETON_BONE1;
						break;
					case 1:
						obj2->var6=sSKELETON_BONE2;
						break;
					case 2:
					case 6:
						obj2->var6=sSKELETON_BONE3;
						break;
					case 3:
						obj2->var6=sSKELETON_BONE5;
						break;
					case 4:
						obj2->var6=sSKELETON_BONE6;
						break;

					default:
					case 7:
					case 5:
						obj2->var6=sSKELETON_BONE7;
						break;
				}	//switch
				//type:RANDPART, randomize done by AI func
			}
		}
		MyDeleteObject(num);//obj->code=NULL;
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;

	default:
		FAIL("skeleton(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //switch

} //skeleton()





static int turtleimpact[8]=
{	sCOMMON_IMPACT1,sCOMMON_IMPACT2,sCOMMON_IMPACT3,sCOMMON_IMPACT4,
	sCOMMON_IMPACT5,sCOMMON_IMPACT6,sCOMMON_IMPACT7,sCOMMON_IMPACT8};

static int turtlefly[8]=
{	sCOMMON_SWISH1,sCOMMON_SWISH2,sCOMMON_SWISH3,sCOMMON_SWISH4,
	sCOMMON_SWISH5,sCOMMON_SWISH6,sCOMMON_SWISH7,sCOMMON_SWISH8};

static int turtlehead[4]=
{	sCOMMON_PLOP1,sCOMMON_PLOP2,sCOMMON_PLOP3,sCOMMON_PLOP4};


void turtleshell(int num)
{
    Omonster *obj;
	int nogoup,nogodown,nogoleft,nogoright,sample;
	int calc,calc2;
	int numo;


    obj=(Omonster *)(&gameobj[num]);

    if (obj->state==sSTART)
    {
//		obj->ypos-=4*65536;
        obj->state=sFLY;
		obj->counter=0;
	} else
	if ((obj->state==sKILL) || (obj->state==sDEACTIVATE))
	{

		MyDeleteObject(num);//obj->code=NULL;		
	} else
		obj->nohit=0;

//to be sure (was fucked up)
	obj->phase=0;
//	obj->curanim=AnimBase[mTURTLE]+mTURTLE_REVSHELL;
	obj->curframe=anims[obj->curanim].framelist[0];


	if ((obj->state==sACTION) || (obj->state==sFLY))
	{
		if (obj->counter==0)
		{
			PlaySample(obj->xpos,obj->ypos,turtlefly[RandFac(7)],0,0);
		}


		if (obj->ypos>level.waterlevel)
		{
			obj->xspeed=(obj->xspeed*7)/8;
			obj->yspeed=(obj->yspeed*7)/8;
		}
		else
			obj->xspeed=(obj->xspeed*63)/64;

		obj->xpos+=obj->xspeed;
		obj->ypos+=obj->yspeed;



		sample=0;
		nogoup=CheckPixel(obj->xpos,obj->ypos-4*65536);
		nogodown=CheckPixel(obj->xpos,obj->ypos+16*65536);
		nogoleft=CheckPixel(obj->xpos-8*65536,obj->ypos);
		nogoright=CheckPixel(obj->xpos+8*65536,obj->ypos);


		if (ABS(obj->xspeed)>1024) {
			obj->ProcPtr=0;
			for (numo=1;numo<NumObjects;numo++)
			if (gameobj[numo].ProcPtr) {
    			if (((gameobj[numo].objtype&oTYPES)==oNORMAL) || ((gameobj[numo].objtype&oTYPES)==oSPECIAL)) {
					calc=PixelCollision(&gameobj[num],&gameobj[numo]);

					if (calc)
					{
						HitObject(numo,obj->creator,-1);

						if (obj->xspeed>0)
							nogoright=1;
						else
						if (obj->xspeed<0)
							nogoleft=1;
					}
				}
			} //collision with objects!
			obj->ProcPtr=&turtleshell;
		} //speed above threshold


		if ((obj->xspeed>0) && nogoright)
		{
			calc=1+(obj->xspeed/65536);

			calc2=CheckVLine((obj->xpos+8*65536)/65536,(obj->ypos/65536)+16-calc,calc);

			if ((calc2<=1) || (calc2>=calc))
			{
				obj->xpos-=obj->xspeed;
				obj->xspeed=-obj->xspeed;
			} else
			{
				obj->ypos-=calc*65536;
				obj->xspeed=(obj->xspeed*15)/16;
			};
		} else
		if ((obj->xspeed<0) && nogoleft)
		{
			calc=1-(obj->xspeed/65536);

			calc2=CheckVLine((obj->xpos-8*65536)/65536,(obj->ypos/65536)+16-calc,calc);

			if ((calc2<=1) || (calc2>=calc))
			{
				obj->xpos-=obj->xspeed;
				obj->xspeed=-obj->xspeed;
			} else
			{
				obj->ypos-=calc*65536;
				obj->xspeed=(obj->xspeed*15)/16;
			};
		};

		if (nogoup && (obj->yspeed<0))
		{
			obj->ypos-=obj->yspeed; //reset to last location!
			obj->yspeed=-obj->yspeed/2;
		} else
		if (nogodown)
		{
			calc=CheckVLine(obj->xpos/65536,obj->ypos/65536,16);

			if (calc)
				obj->ypos-=(16-calc)*65536;
			else
				obj->ypos-=obj->yspeed;

			
			if (obj->yspeed>65536)
				PlaySample(obj->xpos,obj->ypos,turtleimpact[RandFac(7)],0,0);

			obj->yspeed=-(obj->yspeed/2);
		} else
		{
			if (obj->ypos>level.waterlevel)
			{
				obj->yspeed+=level.gravity/4;
				if (obj->yspeed>4*65536) obj->yspeed=4*65536;
				else
				if (obj->yspeed<-4*65536) obj->yspeed=-4*65536;
			}
			else
			{
				obj->yspeed+=level.gravity;
				if (obj->yspeed>8*65536) obj->yspeed=8*65536;
			};
		};
	};

	obj->counter++;
	DrawGameobj(num);

} //turtleshell()






void turtle(int num)	//generic turtle
{
	Omonster *obj;
	int closestplayer;
	int oldy;
	Word8 oldfreeze;
	int hit1,hit2;
	int px,py;


	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		obj->curanim=AnimBase[mTURTLE]+mTURTLE_WALK;
		
		PutWalkerOnGround(num);

		obj->direction=1;
		obj->state=sWALK;
		obj->nohit=0;
		break;

	case sWALK:
		obj->nohit=0;

		obj->xpos+=obj->direction*loadobj[obj->load].xspeed;//default speed

		if (obj->xspeed<0)
			px=-frames[obj->curframe].tx2-frames[obj->curframe].dx;
		else
			px=frames[obj->curframe].dx+frames[obj->curframe].tx2;

		py=frames[obj->curframe].ty2-frames[obj->curframe].ty;

		hit1=CheckPixel(obj->xpos+(px*65536),obj->ypos+(py<<16));

		hit2=CheckPixel(obj->xpos+(px*65536),obj->ypos+((py-16)<<16));

		if ((hit2==1) || (hit1==0) || (tileattr==areaSTOPENEMY))	//bounce of wall
		{
			if (obj->xspeed>=0)
				px=-frames[obj->curframe].tx2-frames[obj->curframe].dx;
			else
				px=frames[obj->curframe].dx+frames[obj->curframe].tx2;

			hit2=CheckPixel(obj->xpos+(px*65536),obj->ypos+((py-16)<<16));

			if (hit2==1)//dont do the direction swap
			{
				obj->state=sWAIT;
			}
			else
			{
				obj->xpos-=obj->xspeed;
				obj->state=sTURN;
				obj->counter=0;
				obj->curanim=AnimBase[mTURTLE]+mTURTLE_INSHELL;
				PlaySample(obj->xpos,obj->ypos,sTURTLE_HIDE,0,0);
			}
		}


		if ((gamecounter&7)==0)
			obj->phase++;
		if (obj->phase>=(int)anims[obj->curanim].numframes)
			obj->phase=0;

		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		DrawGameobj(num);

		if (obj->counter>AISPEED)
		{
			closestplayer=GetNearestPlayer(num,128*128);
			if (closestplayer>=0) 
			{
				if (((obj->direction<0) && (player[closestplayer].xpos<obj->xpos)) ||
					((obj->direction>0) && (player[closestplayer].xpos>obj->xpos)) &&
					(ABS((player[closestplayer].ypos-obj->ypos)/65536)<64))
				{
					//if ((obj->counter>AISPEED) && (Distance<80*80))
					if (Distance<80*80)
						obj->state=sATTACK;
					else
					if ((Distance>90*90) && (player[closestplayer].lastfire<20))
						obj->state=sACTION;
					//else nothing: just walk ;-)
				};
	//			else
	//				obj->state=sACTION;
			};
		};

		obj->counter++;
		if (obj->counter>AISPEED*4)
		{
			obj->state=sIDLE;
			obj->counter=0;
			obj->phase=0;
			obj->curanim=AnimBase[mTURTLE]+mTURTLE_IDLE;
		};


		if ((obj->counter&31)==1)
		{
			switch (RandFac(7)&7)
			{
			case 0:
				PlaySample(obj->xpos,obj->ypos,sTURTLE_SPK1TURT,0,0);
				break;
			case 1:
				PlaySample(obj->xpos,obj->ypos,sTURTLE_SPK2TURT,0,0);
				break;
			case 2:
				PlaySample(obj->xpos,obj->ypos,sTURTLE_SPK3TURT,0,0);
				break;
			case 3:
				PlaySample(obj->xpos,obj->ypos,sTURTLE_SPK4TURT,0,0);
				break;
			default:
				break;
			};
		}; //whatever

		break;

	case sWAIT:
		if ((gamecounter&7)==0)
			obj->phase++;
		if (obj->phase>=(int)anims[obj->curanim].numframes)
			obj->phase=0;

		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		DrawGameobj(num);
		break;


	case sIDLE:
		obj->nohit=0;

		obj->counter++;

		if ((obj->counter&7)==7)
		{
			obj->phase++;

			if (obj->phase==2)
			{
				if (RandFac(1))
					PlaySample(obj->xpos,obj->ypos,sTURTLE_IDLE1,0,0);	
				else
					PlaySample(obj->xpos,obj->ypos,sTURTLE_IDLE2,0,0);	
			} else
			if (obj->phase>=(int)anims[obj->curanim].numframes)
			{	
				obj->phase=0;
				obj->counter=0;
				obj->curanim=AnimBase[mTURTLE]+mTURTLE_WALK;
				obj->state=sWALK;
			};
		};

		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		DrawGameobj(num);
		break;


	case sTURN: //turn cycle at walls
		obj->nohit=4;

		if (obj->counter==0)
		{
			PlaySample(obj->xpos,obj->ypos,sTURTLE_TURN,0,0);
		};
		obj->counter++;

		if ((obj->counter&7)==7)
		{
			obj->phase++;


			if ((obj->phase==3) && (obj->curanim==(int)AnimBase[mTURTLE]+mTURTLE_OUTSHELL))
			{
				PlaySample(obj->xpos,obj->ypos,turtlehead[RandFac(3)],0,0);
			};

			if (obj->phase>=(int)anims[obj->curanim].numframes)
			{	
/*				closestplayer=GetNearestPlayer(num,80*80);
				if (closestplayer>=0)
				{
					obj->phase=anims[obj->curanim].numframes-1;
					obj->counter--;
				} else
*/				{
					obj->phase=0;
					obj->counter=0;
					if (obj->curanim==(int)AnimBase[mTURTLE]+mTURTLE_INSHELL) {
						obj->direction=-obj->direction;
						obj->xspeed=-obj->xspeed;
						obj->curanim=AnimBase[mTURTLE]+mTURTLE_OUTSHELL;
					} else
					{
						obj->state=sWALK;
						obj->curanim=AnimBase[mTURTLE]+mTURTLE_WALK;
					};
				};
			};
		};
		
		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		DrawGameobj(num);
		break; //turn cycle

	case sFREEZE:
		obj->nohit=0;
		
		if (obj->freeze>0) obj->freeze--;

		if (obj->freeze<30)
		{
			oldy=obj->ypos;		obj->ypos+=4*Rand2Fac(16383);
			oldfreeze=obj->freeze;	obj->freeze=(Word8)RandFac(1); //let freeze anim flicker

			DrawGameobj(num);
			obj->freeze=oldfreeze;
            obj->ypos=oldy;
			if (obj->freeze==0) 
			{
				cUNFREEZE(num,2);
				obj->state=obj->oldstate;
			};
		}
		else
			DrawGameobj(num);
		break;

	case sACTION:
		closestplayer=GetNearestPlayer(num,100*100);


		//if ((closestplayer>=0) && (player[closestplayer].lastfire<AISPEED))
		if (closestplayer>=0)
		{ //go into shell if player close, and STAY THERE!
			obj->nohit=4;

			if (obj->curanim!=(int)(AnimBase[mTURTLE]+mTURTLE_INSHELL))
			{
				obj->curanim=AnimBase[mTURTLE]+mTURTLE_INSHELL;
				obj->phase=0;
				PlaySample(obj->xpos,obj->ypos,sTURTLE_HIDE,0,0);
			};

			obj->counter++;
			if ((obj->counter&3)==3) //fast!
			{
				obj->phase++;
				if (obj->phase>=(int)anims[obj->curanim].numframes)
				{
					obj->phase=anims[obj->curanim].numframes-1; //wait it out!

				};
			};

		} else
		{ //get outta shell
			obj->nohit=0;

			if (obj->curanim!=(int)AnimBase[mTURTLE]+mTURTLE_OUTSHELL)
			{
				obj->curanim=AnimBase[mTURTLE]+mTURTLE_OUTSHELL;
				obj->phase=0;
			};

			obj->counter++;
			if ((obj->counter&3)==3) //fast!
			{
				obj->phase++;
				if (obj->phase>=(int)anims[obj->curanim].numframes)
				{
					obj->phase=0; 
					obj->counter=0;
					obj->curanim=AnimBase[mTURTLE]+mTURTLE_WALK;
					obj->state=sWALK;
					//obj->var4=0;
				};
			};
		};				

		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		DrawGameobj(num);
		break;

	case sATTACK:
		obj->nohit=0;

		if (obj->curanim!=(int)(AnimBase[mTURTLE]+mTURTLE_ATTACK))
		{
			obj->curanim=AnimBase[mTURTLE]+mTURTLE_ATTACK;
			obj->phase=0;
		};

		obj->counter++;
		if ((obj->counter&3)==3) //fast!
		{
			obj->phase++;
			if (obj->phase>=(int)anims[obj->curanim].numframes)
			{
				obj->phase=0; //wait it out!
				obj->curanim=AnimBase[mTURTLE]+mTURTLE_WALK;
				obj->state=sWALK;
				obj->counter=0;
			} else
			if (obj->phase==3)
				PlaySample(obj->xpos,obj->ypos,sTURTLE_NECK,0,0);				
			else
			if (obj->phase==6)
				PlaySample(obj->xpos,obj->ypos,sTURTLE_BITE3,0,0);				
		};				
		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		DrawGameobj(num);
		break;


	case sKILL:
		AddExplosion(obj->xpos+Rand2Fac(32767)*16,
					obj->ypos+Rand2Fac(32767)*16,
					AnimBase[mAMMO]+mAMMO_POOF1); 
		AddExplosion(obj->xpos+Rand2Fac(32767)*16,
					obj->ypos+Rand2Fac(32767)*16,
					AnimBase[mAMMO]+mAMMO_BOOM6); 

		//PlaySample(obj->xpos,obj->ypos,sCOMMON_PLOPKORK,0,0);
		

		PlaySample(obj->xpos,obj->ypos,turtlefly[RandFac(7)],0,0);

		obj->ProcPtr=&turtleshell;

		obj->curanim=AnimBase[mTURTLE]+mTURTLE_REVSHELL;
		obj->yspeed=-4*65536;

		obj->objtype=oSPECIAL;
		obj->nohit=3; //dont
		obj->energy=2;

		obj->state=sSTART;	//skip random randpart
		obj->points=100;
		obj->load=aTURTLESHELL;
		return; //!
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;

	default:
		FAIL("turtle(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //switch

} //turtle()




void dragon(int num)
{
	Omonster *obj;
	int closestplayer;
	int oldy;
	Word8 oldfreeze;
	int res;
	Obullets *bobj;

	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		PutObjectOnGround(num);

		obj->state=sSTILL;
		obj->counter=AISPEED*4;
		obj->direction=1;

		obj->curanim=AnimBase[mDRAGON]+mDRAGON_STAND;
		
		break;

	case sSTILL:
		if (obj->counter>0)
		{
			obj->counter--;
			if ((gamecounter&7)==0)
				obj->phase++;
			if (obj->phase>=(int)anims[obj->curanim].numframes)
				obj->phase=0;
			obj->curframe=anims[obj->curanim].framelist[obj->phase];


			if (obj->counter<2*AISPEED)
			{
			closestplayer=GetNearestPlayer(num,256*256);
			if (closestplayer>=0) 
			{
				if ((obj->direction<0) && (obj->xpos>player[closestplayer].xpos))
					obj->state=sATTACK; //else fast turn!
				else
				if ((obj->direction>0) && (obj->xpos<player[closestplayer].xpos))
					obj->state=sATTACK; //else fast turn!

				obj->counter=0;
				obj->curanim=AnimBase[mDRAGON]+mDRAGON_ATTACK;
				obj->phase=0;
			};
			};

		} else
		{
			obj->counter=0;
			obj->state=sTURN;
			obj->phase=0;
			obj->curanim=AnimBase[mDRAGON]+mDRAGON_TURN;
			obj->curframe=anims[obj->curanim].framelist[obj->phase];
		};

		DrawGameobj(num);
		break;

	case sTURN:
		obj->counter++;
		if (obj->counter>3)
		{
			obj->counter=0;
			obj->phase++;
			if (obj->phase>=(int)anims[obj->curanim].numframes)
			{
				obj->phase=anims[obj->curanim].numframes-1;
				obj->curframe=anims[obj->curanim].framelist[obj->phase];
				DrawGameobj(num);
				obj->direction=-obj->direction;
				obj->state=sSTILL;
				obj->curanim=AnimBase[mDRAGON]+mDRAGON_STAND;
				obj->counter=AISPEED*4;
			} else
			{
				obj->curframe=anims[obj->curanim].framelist[obj->phase];
				DrawGameobj(num);
			};
		
		} else
		{
			obj->curframe=anims[obj->curanim].framelist[obj->phase];
			DrawGameobj(num);
		};
		break;	
	

	case sATTACK:
		obj->counter++;
		if ((obj->counter&3)==0)
		{
			obj->phase++;
			if (obj->phase>=(int)anims[obj->curanim].numframes)
			{
				obj->phase=0;
				obj->state=sSTILL;
				obj->counter=AISPEED*3;
				obj->curanim=AnimBase[mDRAGON]+mDRAGON_STAND;
			};
		};


		if ((obj->counter>6*4) && (obj->counter<10*4))
		{
			if ((obj->counter%3)==0)
			{
				res=AddBullet(num,aBULLET);

				if (res>=0)
				{
				//DRAGONBULLET
					bobj=(Obullets *)(&gameobj[res]);
					bobj->ProcPtr=&flamebullet;
					bobj->xspeed=2*65536;
					bobj->xacc=8192;
					bobj->force=1;
					bobj->objtype=oENEMYBULLET;
					//obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS2;
					bobj->curanim=AnimBase[mAMMO]+mAMMO_GUN6;
					bobj->type=bullFLAMER;
					bobj->lighttype=1;
					bobj->lifetime=AISPEED/2;
				};

			};
		};

		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);

		break;

	case sFREEZE:
		obj->var4=0;
		
		if (obj->freeze>0) obj->freeze--;

		if (obj->freeze<30)
		{
			oldy=obj->ypos;		obj->ypos+=4*Rand2Fac(16383);
			oldfreeze=obj->freeze;	obj->freeze=(Word8)RandFac(1); //let freeze anim flicker

			DrawGameobj(num);
			obj->freeze=oldfreeze;
            obj->ypos=oldy;
			if (obj->freeze==0) 
			{
				cUNFREEZE(num,2);
				obj->state=obj->oldstate;
			};
		} else
			DrawGameobj(num);
		break;


	case sKILL: 
		cKILL(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;

	default:
		FAIL("dragon(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}
} //dragon()



void bat(int num)
{
	Omonster *obj;
	int closestplayer;
	int wantx,wanty;
	int oldy;
	Word8 oldfreeze;
	int dist,dx,dy;

	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		obj->state=sSTILL;
		obj->direction=1;

		obj->curanim=AnimBase[mBAT]+mBAT_HANG;
		break;

	case sSTILL:
		obj->counter++;

		if ((obj->counter&7)==0)
		{
			obj->counter=0;
			obj->phase++;
			if (obj->phase>=(int)anims[obj->curanim].numframes)
				obj->phase=0;
			
			closestplayer=GetNearestPlayer(num,128*128);
			if (closestplayer>=0) 
			{
				obj->state=sTURN;
				obj->var1=closestplayer;
				obj->phase=0;
				obj->curanim=AnimBase[mBAT]+mBAT_STARTFLY;

//				PlaySample(obj->xpos,obj->ypos,sBAT_SCREAM,128,0);
			}

			obj->curframe=anims[obj->curanim].framelist[obj->phase];
		}
		DrawGameobj(num);
		break;

	case sFREEZE:
		obj->var4=0;
		
		if (obj->freeze>0) obj->freeze--;

		if (obj->freeze<30)
		{
			oldy=obj->ypos;		obj->ypos+=4*Rand2Fac(16383);
			oldfreeze=obj->freeze;	obj->freeze=(Word8)RandFac(1); //let freeze anim flicker

			DrawGameobj(num);
			obj->freeze=oldfreeze;
            obj->ypos=oldy;
			if (obj->freeze==0) 
			{
				cUNFREEZE(num,2);
				obj->state=obj->oldstate;
			};
		} else
		DrawGameobj(num);
		break;


	case sTURN:
		obj->channel=PlayLoopSample(obj->xpos,obj->ypos,sBAT_BATFLY1,43,0,obj->channel);

		obj->counter++;
		if (obj->counter>3)
		{
			obj->counter=0;
			obj->phase++;
			if (obj->phase>=(int)anims[obj->curanim].numframes)
			{
				obj->phase=0;
				obj->state=sFLY;
				obj->counter=0;
				obj->curanim=AnimBase[mBAT]+mBAT_FLY;
			}
		}

		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		DrawGameobj(num);
		break;

	case sFLY:
		obj->channel=PlayLoopSample(obj->xpos,obj->ypos,sBAT_BATFLY1,43,0,obj->channel);

		wantx=player[obj->var1].xpos;
		wanty=player[obj->var1].ypos;


		dx=(wantx-obj->xorg)/65536;
		dy=(wanty-obj->yorg)/65536;
		dist=dx*dx+dy*dy;

		if (dist>8*32*8*32) 
		{	//return to base
			wantx=obj->xorg;
			wanty=obj->yorg;
		}

		if (wantx<obj->xpos)
			obj->xpos-=RandFac(32767)*3;
		else
		if (wantx>obj->xpos)
			obj->xpos+=RandFac(32767)*3;

		obj->ypos+=obj->yspeed;

		obj->counter++;
		if (obj->counter>7)
		{
			if (wantx<obj->xpos-65536)
				obj->direction=-1;
			else
			if (wantx>obj->xpos+65536)
				obj->direction=1;

			if (wanty<obj->ypos)
				obj->yspeed=-(int)RandFac(32767)*4;
			else
			if (wanty>obj->ypos)
				obj->yspeed=(int)RandFac(32767)*4;

			obj->counter=0;
			obj->phase++;

//			if (obj->phase==2)
//				PlaySample(obj->xpos,obj->ypos,sBAT_FOEW1,128,0);

			if (obj->phase>=(int)anims[obj->curanim].numframes)
				obj->phase=0;
		}
		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		DrawGameobj(num);
		break;

	case sKILL: 
		cKILL(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;

	default:
		FAIL("bat(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	};
} //bat()






void cVINE(int num)
{
	Omonster *obj;
	int endx,endy;
	int px,py;

	obj=(Omonster *)&gameobj[num];


	if (obj->state==sSTART)
	{
		obj->var2=128; //length

		obj->var3=obj->var2*256;
		obj->var4=0;
		obj->state=sACTION;
	};

	if (obj->state==sDEACTIVATE)
	{
		cDEACTIVATE(num);
		return;
	};

	if (obj->state==sSLEEP)
	{	//after you jumped off it!
		obj->counter--;
		if (obj->counter<0)
			obj->state=sACTION;
	};


	if (obj->var3>0)
	{
		obj->var4-=32;
	} else
	{
		obj->var4+=32;
	};


	obj->var3+=obj->var4;


	obj->var1=(obj->var2*256+obj->var3)/(obj->var2*2);

	endx=obj->var3/64;

	endy=obj->var2-ABS(obj->var3/2048);
	
/*
	x/y positions are the end of the vine, where you can actually grab
	on and where collision is happening with.

	OR: don't handle collision thru normal collision routines:
	handle collision here, where the vine is subdivided and the elements
	are checked against the nearest players

*/

	
	px=obj->xorg+endx*(32+endy*endy);
	py=obj->yorg+endy*65536+20*65536;


	if (obj->var4<0)
		obj->xspeed=-4*65536;
	else
		obj->xspeed=4*65536;

	obj->yspeed=obj->ypos-py;

	obj->xpos=px;	//collision!
	obj->ypos=py;


	//frame=num off object so that the vine can be referenced back!
	AddSpecialSprite(obj->xorg,obj->yorg,SPRITELAYER,spriteVINE,num,0);


	//donot draw this sprite! just for grabbing on!

} //cVINE()







void cSTEADYLIGHT(int num)
{
	if (gameobj[num].state==sDEACTIVATE)
		cDEACTIVATE(num);
} //cSTEADYLIGHT()





void cPULZELIGHT(int num)
{
	Omonster *obj;

	obj=(Omonster *)&gameobj[num];

	if (obj->state==sSTART)
	{
		obj->var1=GetEventParm(obj->xpos/(32*65536),obj->ypos/(32*65536),0,8);
		if (obj->var1==0)
			obj->var1=16;
		obj->var2=64*GetEventParm(obj->xpos/(32*65536),obj->ypos/(32*65536),8,4);
		obj->state=sSLEEP;
	};

	obj->light=11+sintable(gamecounter*obj->var1+obj->var2)/6000;

	if (obj->state==sDEACTIVATE)
		cDEACTIVATE(num);

} //cPULZELIGHT()


static void cSYNCEDLIGHT(int num)
{
	Omonster *obj;
//	int x,y,tile,calc;

	obj=(Omonster *)&gameobj[num];
/*

	x=obj->xpos/(32*65536);
	y=obj->ypos/(32*65536);

	tile=GetSpriteLayerTile(x, y);

	calc=GetDestructAnimTileFrames(tile);
*/

	//
	obj->light=11+sintable(gamecounter*16)/6000;

	if (obj->state==sDEACTIVATE)
		cDEACTIVATE(num);

} //cSYNCEDLIGHT



void cFLICKERLIGHT(int num)
{
	Omonster *obj;
	int volume;


	obj=(Omonster *)(&gameobj[num]);

	if (obj->state==sDEACTIVATE)
		cDEACTIVATE(num);


	if (obj->state==sSTART)
	{
		obj->var1=GetEventParm(obj->xpos/(32*65536),obj->ypos/(32*65536),0,8);

		obj->state=sSLEEP;
	};

	if (obj->var1)
	{
		volume=128+
				sintable(gamecounter*4)/8192+
				sintable(gamecounter*6)/8192;

		obj->channel=PlayLoopSample(obj->xpos,obj->ypos,sCOMMON_BURNIN,volume,0,obj->channel);
	};

} //cFLICKERLIGHT()




void labrat(int num)
{
	Omonster *obj;
	int oldy;
	Word8 oldfreeze;
	int nump,dx,px,py,hit1,hit2;
//	int calc;
//	int animspeed;


	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		obj->curanim=AnimBase[mLABRAT]+mLABRAT_WALK;
		PutWalkerOnGround(num);
		//obj->xacc=0;
		obj->state=sWALK;
		obj->var4=30+RandFac(31);
		break;

	case sWALK:
		obj->curanim=AnimBase[mLABRAT]+mLABRAT_WALK;
		cWALK(num);

		obj->counter++; 

		if (obj->counter>AISPEED*8)
		{
			obj->counter=0;
			obj->phase=0;
			obj->state=sIDLE;
			obj->var4=RandFac(3);
			return;
		};

		if (RandFac(31)==0)
		{
			nump=RandFac(3);
			switch ((RandFac(3))&3)
			{
			case 0:
				PlaySample(obj->xpos,obj->ypos,sLABRAT_MOUSE1,0,0);
				break;
			case 1:
				PlaySample(obj->xpos,obj->ypos,sLABRAT_MOUSE2,0,0);
				break;
			case 2:
				PlaySample(obj->xpos,obj->ypos,sLABRAT_MOUSE3,0,0);
				break;

			default:
				break;
			}
		}; //sample

		if (obj->counter>AISPEED*2)
		{
			nump=GetNearestPlayer(num,128*128);
			if (nump>=0)
			{
				dx=player[nump].xpos-obj->xpos;
				if (((obj->direction<0) && (dx<0)) ||
					((obj->direction>0) && (dx>0)))
				{
					obj->state=sACTION;
					obj->xspeed=obj->direction*3*65536; //fast!
					obj->counter=0;
					obj->phase=0;
					obj->curanim=AnimBase[mLABRAT]+mLABRAT_ATTACK;
					obj->ypos-=4*65536;
					obj->yspeed=-3*65536;
				}
			}
		}
		break;


	case sACTION:	//attack!
		
		obj->xpos+=obj->xspeed;

		//CHECKING FOR BOUNCING OF WALLS OR RAVINES

		if (obj->direction<0)
			px=-frames[obj->curframe].tx2-frames[obj->curframe].dx;
		else
			px=frames[obj->curframe].dx+frames[obj->curframe].tx2;

		//py=frames[obj->curframe].ty2+frames[obj->curframe].dy;

		//hit1=CheckPixel(obj->xpos+(px*65536),obj->ypos+(20<<16));
		hit2=CheckPixel(obj->xpos+(px*65536),obj->ypos-8*65536);

		if (hit2==1)	//bounce of wall
		{
			obj->xpos-=obj->xspeed;
/*
			obj->xspeed=-obj->xspeed/4;
			obj->direction=-obj->direction;
*/
			obj->xspeed=0;
		}

		py=frames[obj->curframe].ty2+frames[obj->curframe].dy;

		if (obj->yspeed>0)
			py+=4; //land correctly
		//t1=CheckPixel(obj->xpos+(px*65536),obj->ypos+(py*65536));
		hit1=CheckPixel(obj->xpos+15*65536,obj->ypos+py*65536);

		if (hit1==0)
		{
			if (obj->ypos>level.waterlevel)
				obj->yspeed+=level.gravity/4;
			else
				obj->yspeed+=level.gravity;
			if (obj->yspeed>6*65536)
				obj->yspeed=6*65536;
			obj->ypos+=obj->yspeed;
		}
		else
		{
/*
			if (GetEvent(obj->xpos,obj->ypos-8*65536)==areaSTOPENEMY)
			{
				obj->counter=-AISPEED*2;
				obj->state=sEXTRA;
				//save speeds for reverse jump!
	
			} else
*/			{
				obj->yacc=0;
				obj->yspeed=0;
			}
		}

		obj->counter++;
		if (obj->counter>6)
		{
			obj->counter=0;
			obj->phase++;
			if (obj->phase>=5)
			{
				if (hit1)
					obj->state=sLAND;
				else
					obj->phase=5;	//fall anim
			} else
			if (obj->phase==3)
				PlaySample(obj->xpos,obj->ypos,sLABRAT_BITE,0,0);
		}

		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		DrawGameobj(num);
		break;

	case sLAND:
		PutWalkerOnGround(num);

		obj->yspeed=0;
		obj->xspeed=0;
		obj->counter++;
		if (obj->counter>6)
		{
			obj->counter=0;
			obj->phase++;
			if (obj->phase>=(int)anims[obj->curanim].numframes)
			{
				obj->phase=0;
				obj->state=sWALK;
				obj->xspeed=0; //cWALK resets this
			}
		}
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;


	case sEXTRA: //wrong jump ;-)
		obj->counter++;
		if (obj->counter<0)
		{
			obj->curanim=AnimBase[mLABRAT]+mLABRAT_IDLE;

			obj->phase=((AISPEED*2+obj->counter)/6)%anims[obj->curanim].numframes;
		}
		else
		if (obj->counter==0)
		{
			obj->curanim=AnimBase[mLABRAT]+mLABRAT_ATTACK;

			obj->direction=-obj->direction;
			obj->xspeed=-obj->xspeed;
			obj->yspeed=-obj->yspeed;
		}
		else
		{
			obj->xpos+=obj->xspeed;

			if (obj->ypos>level.waterlevel)
				obj->yspeed+=level.gravity/2;
			else
				obj->yspeed+=level.gravity*2;
			obj->ypos+=obj->yspeed;
		}

		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		DrawGameobj(num);
		break;

	case sIDLE:
		obj->curanim=AnimBase[mLABRAT]+mLABRAT_IDLE;

		obj->counter++;
		if (obj->counter>6)
		{
			obj->phase++;
			if ((obj->phase==5) && (InViewArea(obj->xpos,obj->ypos))) 
			{
				if (RandFac(1)==1)
					PlaySample(obj->xpos,obj->ypos,sLABRAT_EYE3,0,0);
				else
					PlaySample(obj->xpos,obj->ypos,sLABRAT_EYE2,0,0);				
				
			}
			obj->counter=0;
		}

		if (obj->phase>=(int)anims[obj->curanim].numframes)
		{
			obj->var4--;
			if (obj->var4>0)
			{
				obj->phase=0;
			} else
			{
				obj->state=sWALK;
				obj->curanim=AnimBase[mLABRAT]+mLABRAT_WALK;
				obj->phase=0;
				obj->xspeed=0; //cWALK resets this
			}
		}

		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		DrawGameobj(num);
		break;

	case sWAIT:
		if ((gamecounter&7)==0)
			obj->phase++;
		if (obj->phase>=(int)anims[obj->curanim].numframes)
			obj->phase=0;
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;


	case sFREEZE:
		if (obj->freeze>0) obj->freeze--;

		if (obj->freeze<30)
		{
			oldy=obj->ypos;		obj->ypos+=4*Rand2Fac(16383);
			oldfreeze=obj->freeze;	obj->freeze=(Word8)RandFac(1); //let freeze anim flicker

			DrawGameobj(num);
			obj->freeze=oldfreeze;
            obj->ypos=oldy;
			if (obj->freeze==0) 
			{
				cUNFREEZE(num,2);
				obj->state=obj->oldstate;
				if (obj->state==sWALK)
					obj->xspeed=0; //cWALK resets this
			}
		} else
		DrawGameobj(num);
		break;

	case sKILL:
		cKILL(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;

	default:
		DebugXMessage("labrat(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}	//switch

} //labrat()







static int suckerplopsamps[4]={
	sSUCKER_PLOPSEQ1,
	sSUCKER_PLOPSEQ2,
	sSUCKER_PLOPSEQ3,
	sSUCKER_PLOPSEQ4};

void sucker(int num)
{
	Omonster *obj;
	int oldy;
	Word8 oldfreeze;


	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		PutWalkerOnGround(num);
		obj->state=sWALK;
		obj->var1=20000+RandFac(4095);
		break;

	case sWAKE:
	case sWALK:

		if ((gamecounter&7)==0)
		{
			if ((obj->phase==6) || (obj->phase==7))
				obj->xspeed=obj->direction*6*65536;
			else
			if ((obj->phase>=10) && (obj->phase<=13))
				obj->xspeed=obj->direction*6*65536;
		} else
			obj->xspeed=obj->direction*1;

		cWALK(num);
		
		if ((gamecounter&7)==0)
		{
			if (obj->phase==0) 
				PlaySample(obj->xpos,obj->ypos,sSUCKER_PINCH1,0,obj->var1);	
			else
			if (obj->phase==7)
				PlaySample(obj->xpos,obj->ypos,sSUCKER_PINCH3,0,obj->var1);
			else
			if ((obj->phase==3) || (obj->phase==8))
				PlaySample(obj->xpos,obj->ypos,suckerplopsamps[RandFac(3)],40,obj->var1);
		}
		break;

	case sWAIT:
		if ((gamecounter&7)==0)
			obj->phase++;
		if (obj->phase>=(int)anims[obj->curanim].numframes)
			obj->phase=0;
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;

	case sSLEEP:
		cSLEEP(num,16*16);
		break;

	case sFREEZE:
		if (obj->freeze>0) obj->freeze--;
		if (obj->freeze==0) obj->state=obj->oldstate;

		if (obj->freeze<30)
		{
			oldy=obj->ypos;		obj->ypos+=4*Rand2Fac(16383);
			oldfreeze=obj->freeze;	obj->freeze=(Word8)RandFac(1); //let freeze anim flicker

			DrawGameobj(num);
			obj->freeze=oldfreeze;
            obj->ypos=oldy;
		} else
		DrawGameobj(num);

		break;

	case sKILL:
		cKILL(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;

	default:
		FAIL("sucker(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //switch
} //sucker()



void floatsucker(int num)
{
	Omonster *obj;
//	int check;
	Word8 oldfreeze;
	int oldy;

	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		obj->state=sFLOAT;
		obj->var1=AISPEED*4;
		break;

	case sFLOAT:
		obj->curanim=AnimBase[mSUCKER]+mSUCKER_FLOAT;
		obj->counter++;

		obj->xpos=obj->xorg+sintable(obj->counter*8)*16;
		if ((obj->counter&127)<63)
			obj->direction=-1;
		else
			obj->direction=+1;
		obj->ypos=obj->yorg+costable(obj->counter*8)*16;

		obj->phase=(obj->counter/6)%anims[obj->curanim].numframes;
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;

	case sHIT:
		if (obj->curanim!=(int)AnimBase[mSUCKER]+mSUCKER_DEFLATE)
		{
			PlaySample(obj->xpos,obj->ypos,sSUCKER_FART,128,0);
			obj->curanim=AnimBase[mSUCKER]+mSUCKER_DEFLATE;
		};

		obj->counter++;
		if (obj->counter>6)
		{
			obj->phase++;
			obj->counter=0;
			if (obj->phase>=(int)anims[obj->curanim].numframes)
			{
				obj->phase=0;
				obj->curanim=AnimBase[mSUCKER]+mSUCKER_FALL;
				obj->state=sFLY;
			};		
		};

		if ((gamecounter&3)==0)
			AddExplosion(obj->xpos,obj->ypos,AnimBase[mSUCKER]+mSUCKER_GAS); //smoke particle

		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		obj->yspeed=2048;

		obj->ypos+=obj->yspeed;

		if (obj->xspeed<0)
		{
			obj->xspeed-=1024;
			if (obj->xspeed<-6*65536) obj->xspeed=-6*65536;

			obj->direction=-1;
			if (CheckPixel(obj->xpos-24*65536,obj->ypos))
			{
				obj->xpos-=obj->xspeed;
				obj->xspeed=0;
				obj->direction=1;
				obj->state=sFLY;
			}
		}
		else
		{
			obj->xspeed+=1024;
			if (obj->xspeed>6*65536) obj->xspeed=6*65536;

			obj->direction=+1;

			if (CheckPixel(obj->xpos+24*65536,obj->ypos))
			{
				obj->xpos-=obj->xspeed;
				obj->xspeed=0;
				obj->direction=-1;
				obj->state=sFLY;
			}
		}

		obj->xpos+=obj->xspeed;
		
		DrawGameobj(num);
		break;


	case sFLY:

		obj->counter++;
		if (obj->counter>6)
		{
			AddExplosion(obj->xpos,obj->ypos,AnimBase[mSUCKER]+mSUCKER_GAS); //smoke particle

			obj->phase++;
			obj->counter=0;
			if (obj->phase>=(int)anims[obj->curanim].numframes)
				obj->phase=0;
		};
		obj->curframe=anims[obj->curanim].framelist[obj->phase];


		obj->yspeed+=4096;
		if (obj->yspeed>8*65536)
			obj->yspeed=8*65536;

		obj->ypos+=obj->yspeed;

		obj->xspeed=(obj->xspeed*15)/16;
		obj->xpos+=obj->xspeed;

		//check=(frames[obj->curframe].ty2+frames[obj->curframe].ty)*65536;

		if (CheckPixel(obj->xpos+obj->xspeed*2,obj->ypos+23*65536))
		{
			PlaySample(obj->xpos,obj->ypos,sCOMMON_SPLUT,128,0);
			obj->nohit=0;
			obj->ProcPtr=&sucker;
			obj->xspeed=obj->direction*65536;
			obj->energy=1;
			obj->curanim=AnimBase[mSUCKER]+mSUCKER_GROUND;
			obj->state=sSTART;
			obj->yspeed=0;
			obj->objtype=oNORMAL+oENEMY; //clear!
		};

		DrawGameobj(num);
		break;

	case sFREEZE:
		if (obj->freeze>0) obj->freeze--;

		if (obj->freeze<30)
		{
			oldy=obj->ypos;		obj->ypos+=4*Rand2Fac(16383);
			oldfreeze=obj->freeze;	obj->freeze=(Word8)RandFac(1); //let freeze anim flicker

			DrawGameobj(num);
			obj->freeze=oldfreeze;
            obj->ypos=oldy;
			if (obj->freeze==0) 
			{
				cUNFREEZE(num,2);
				obj->state=obj->oldstate;
			};
		} else
		DrawGameobj(num);
		break;


	case sKILL:
		cKILL(num);
		break;
	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;
	default:
		FAIL("floatsucker(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //switch
} //floatsucker()



void sparks(int num)
{
	Omonster *obj;
//	int check;
	int dx,dy;
	int wantsx,wantsy;
	int maxspeed;
	int nump;
	int move;
	Word8 oldfreeze;
	int oldy;


	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		obj->state=sFLOAT;
		break;

	case sFLOAT:
		obj->counter++;
		nump=GetNearestPlayer(num,256*256);
		if (nump<0)
		{
			obj->xspeed=(obj->xspeed*31)/32;
			obj->yspeed=(obj->yspeed*31)/32;

			obj->phase=(obj->counter/6)%anims[obj->curanim].numframes;
			obj->curframe=anims[obj->curanim].framelist[obj->phase];
			DrawGameobj(num);
			return;
		};

		dx=(player[nump].xpos-obj->xpos);
		dy=(player[nump].ypos-obj->ypos);

		if (((dx>0) && (player[nump].direction<0)) ||
			((dx<0) && (player[nump].direction>0)))
			move=0;
		else
		if ((ABS(dx)>16*65536) || (ABS(dy)>16*65536))
			move=1;
		else
			move=0;


		if (move==0)
		{
			obj->xspeed=(obj->xspeed*31)/32;
			obj->yspeed=(obj->yspeed*31)/32;

		} else
		{
			wantsx=dx;
			wantsy=dy;


			maxspeed=(2+level.difficulty)*65536;

			if (wantsx>maxspeed)
				wantsx=maxspeed;
			else
			if (wantsx<-maxspeed)
				wantsx=-maxspeed;

			if (wantsy>maxspeed)
				wantsy=maxspeed;
			else
			if (wantsy<-maxspeed)
				wantsy=-maxspeed;

			if (obj->xspeed<wantsx)
				obj->xspeed+=1024*4;
			else
			if (obj->xspeed>wantsx)
				obj->xspeed-=1024*4;
			else
				obj->xspeed=wantsx;


			if (obj->yspeed<wantsy)
				obj->yspeed+=1024*4;
			else
			if (obj->yspeed>wantsy)
				obj->yspeed-=1024*4;
			else
				obj->yspeed=wantsy;

		};	
/*
		if (obj->xspeed<-4096)
			obj->direction=1;
		else
		if (obj->xspeed>4096)
			obj->direction=-1;
*/
		obj->xpos+=obj->xspeed;
		obj->ypos+=obj->yspeed;		

		if (dx>0) obj->direction=-1;
		else
		if (dx<0) obj->direction=1;

		obj->phase=(obj->counter/6)%anims[obj->curanim].numframes;
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);

		break;	

	case sFREEZE:
		if (obj->freeze>0) obj->freeze--;

		if (obj->freeze<30)
		{
			oldy=obj->ypos;		obj->ypos+=4*Rand2Fac(16383);
			oldfreeze=obj->freeze;	obj->freeze=(Word8)RandFac(1); //let freeze anim flicker

			DrawGameobj(num);
			obj->freeze=oldfreeze;
            obj->ypos=oldy;
			if (obj->freeze==0) 
			{
				cUNFREEZE(num,2);
				obj->state=obj->oldstate;
			};
		} else
		DrawGameobj(num);
		break;


	case sKILL:
		cKILL(num);
		break;
	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;
	default:
		FAIL("sparks(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //switch
} //sparks()




void helmut(int num)
{
	Omonster *obj;
	int oldy;
	Word8 oldfreeze;
	int nump,dx,px,py,hit1,hit2;
//	int animspeed;


	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		PutWalkerOnGround(num);
		//obj->xacc=0;
		obj->state=sWALK;
		obj->var4=30+RandFac(31);
		break;

	case sWALK:
		obj->curanim=AnimBase[mHELMUT]+mHELMUT_WALK;
		obj->xspeed=obj->direction*((65536+sintable((ABS(obj->phase-11)*32)&511)       )/2);
		cWALK(num);

		obj->counter++; 

		if (obj->counter>AISPEED*8)
		{
			obj->counter=0;
			obj->phase=0;
			obj->state=sIDLE;
			obj->var4=RandFac(3);
			return;
		};

		if ((obj->counter>AISPEED*2) && (level.difficulty==2))
		{
			nump=GetNearestPlayer(num,96*96);
			if (nump>=0)
			{
				dx=player[nump].xpos-obj->xpos;
				if (((obj->direction<0) && (dx<0)) ||
					((obj->direction>0) && (dx>0)))
				{
					obj->state=sACTION;
					obj->xspeed=obj->direction*3*65536; //fast!
					obj->counter=0;
					obj->phase=0;
					//obj->curanim=AnimBase[mHELMUT]+mHELMUT_WALK;
					obj->ypos-=4*65536;
					obj->yspeed=-4*65536;

				};
			};
		};
		break;

	case sWAIT:
		obj->phase=(Word8)(gamecounter/8)%anims[obj->curanim].numframes;
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;

	case sACTION:	//attack!
		obj->xpos+=obj->xspeed;

		//CHECKING FOR BOUNCING OF WALLS OR RAVINES

		if (obj->direction<0)
			px=-frames[obj->curframe].tx2-frames[obj->curframe].dx;
		else
			px=frames[obj->curframe].dx+frames[obj->curframe].tx2;

		//py=frames[obj->curframe].ty2+frames[obj->curframe].dy;

		//hit1=CheckPixel(obj->xpos+(px*65536),obj->ypos+(20<<16));
		hit2=CheckPixel(obj->xpos+(px*65536),obj->ypos);

		if ((hit2==1) || (tileattr==areaSTOPENEMY))	//bounce of wall
		{
			obj->xpos-=obj->xspeed;
			obj->xspeed=-obj->xspeed/4;
			obj->direction=-obj->direction;
		};

		py=frames[obj->curframe].ty2+frames[obj->curframe].dy;
		//t1=CheckPixel(obj->xpos+(px*65536),obj->ypos+(py*65536));
		hit1=CheckPixel(obj->xpos+(px*65536),obj->ypos+py*65536);

		if (hit1==0)
		{
			if (obj->ypos>level.waterlevel)
				obj->yspeed+=level.gravity/2;
			else
				obj->yspeed+=level.gravity*2;
			if (obj->yspeed>6*65536)
				obj->yspeed=6*65536;
		}
		else
		{
			obj->yacc=0;
			obj->yspeed=0;
			obj->state=sWALK;
			obj->xspeed=0; //cWALK resets this
			obj->counter=0;
		}

		obj->ypos+=obj->yspeed;

		obj->counter++;
		if (obj->counter>6)
		{
			obj->counter=0;
			obj->phase++;
			if (obj->phase>=(int)anims[obj->curanim].numframes)
			{
				obj->phase=0;
			};
		};

		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		DrawGameobj(num);
		break;

	case sIDLE:
		obj->curanim=AnimBase[mHELMUT]+mHELMUT_STAND;

		obj->counter++;
		if (obj->counter>6)
		{
			obj->phase++;
			obj->counter=0;
		};

		if (obj->phase>=anims[obj->curanim].numframes)
		{
			obj->var4--;
			if (obj->var4>0)
			{
				obj->phase=0;
			} else
			{
				obj->state=sWALK;
				obj->curanim=AnimBase[mLABRAT]+mLABRAT_WALK;
				obj->phase=0;
			};
		};

		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		DrawGameobj(num);

		break;

	case sFREEZE:
		if (obj->freeze>0) obj->freeze--;

		if (obj->freeze<30)
		{
			oldy=obj->ypos;		obj->ypos+=4*Rand2Fac(16383);
			oldfreeze=obj->freeze;	obj->freeze=(Word8)RandFac(1); //let freeze anim flicker

			DrawGameobj(num);
			obj->freeze=oldfreeze;
            obj->ypos=oldy;
			if (obj->freeze==0) 
			{
				cUNFREEZE(num,2);
				obj->state=obj->oldstate;
			};
		} else
		DrawGameobj(num);
		break;

	case sKILL:
		cKILL(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;

	default:
		FAIL("helmut(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //switch

} //helmut()



void fatchick(int num)
{
	Omonster *obj;
	int oldy;
	Word8 oldfreeze;
	int nump,dx;


	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		PutWalkerOnGround(num);
		obj->state=sWALK;
		break;

	case sWAKE:
	case sWALK:
		
		obj->xspeed=obj->direction*((65536+sintable((ABS(obj->phase-11)*32)&511)       )/2);

		cWALK(num);
		obj->counter++; 

		if (level.difficulty>0)
		{	
			nump=GetNearestPlayer(num,80*80);
			if (nump>=0)
			{
				//turns around when player is close
				dx=player[nump].xpos-obj->xpos;

				if (dx>16*65536)
					obj->direction=1;
				else
				if (dx<-16*65536)
					obj->direction=-1;
			};
		};


		if (obj->counter>AISPEED*2)
		{
			nump=GetNearestPlayer(num,40*40);
			if (nump>=0)
			{
				dx=player[nump].xpos-obj->xpos;
				if (((obj->direction<0) && (dx<0)) ||
					((obj->direction>0) && (dx>0)))
				{
					obj->state=sACTION;
					obj->xspeed=0; //fast!
					obj->counter=0;
					obj->phase=0;
					obj->curanim=AnimBase[mFATCHK]+mFATCHK_ATTACK;
					obj->var1=nump;
				};
			};
		};
		break;

	case sWAIT:
		obj->phase=(Word8)(gamecounter/8)%anims[obj->curanim].numframes;
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;

	case sACTION:
		obj->counter++;

		if (obj->counter>4)
		{
			obj->phase++;
			obj->counter=0;

			if (obj->phase==5)
			{
//				if ((player[obj->var1].hit<=0) && 
//					(player[obj->var1].invincibility==0))

				switch((RandFac(3))&3)
				{
				case 0:
					PlaySample(obj->xpos,obj->ypos,sFATCHK_HIT1,0,0);
					break;
				case 1:
					PlaySample(obj->xpos,obj->ypos,sFATCHK_HIT2,0,0);
					break;
				default:
					PlaySample(obj->xpos,obj->ypos,sFATCHK_HIT3,0,0);
					break;
				};
			};
			if (obj->phase>=anims[obj->curanim].numframes)
			{
				obj->phase=0;
				obj->curanim=AnimBase[mFATCHK]+mFATCHK_WALKCYC;
				obj->state=sWALK;
				obj->counter=0;
			};
		};


		if ((obj->phase>=5) && (obj->phase<=6))
		{
			dx=(player[obj->var1].xpos-obj->xpos)/65536;
			
			if ((player[obj->var1].hit>0) && (ABS(dx)<64) )
			{
				player[obj->var1].xspeed=obj->direction*16*65536;
				player[obj->var1].yspeed=-8*65536;
				player[obj->var1].bemoved=30;
				player[obj->var1].idletime=0;
			};
		};

		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		DrawGameobj(num);
		break;
	case sSLEEP:
		cSLEEP(num,16*16);
		break;

	case sFREEZE:
		if (obj->freeze>0) obj->freeze--;
		if (obj->freeze==0) obj->state=obj->oldstate;

		if (obj->freeze<30)
		{
			oldy=obj->ypos;		obj->ypos+=4*Rand2Fac(16383);
			oldfreeze=obj->freeze;	obj->freeze=(Word8)RandFac(1); //let freeze anim flicker

			DrawGameobj(num);
			obj->freeze=oldfreeze;
            obj->ypos=oldy;
		} else
		DrawGameobj(num);

		break;

	case sKILL:
		cKILL(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;

	default:
		FAIL("fatchick(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //switch
} //fatchick()



void fencer(int num)
{
	Omonster *obj;
	int oldy;
	Word8 oldfreeze;
	int py,hit,closestplayer;


	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		PutObjectOnGround(num);
		obj->state=sSTILL;
		break;

	case sSTILL:
		closestplayer=GetNearestPlayer(num,512*256);
		if (closestplayer>=0) 
		{
			if (player[closestplayer].xpos<obj->xpos)
				obj->direction=-1;
			else
			if (player[closestplayer].xpos>obj->xpos)
				obj->direction=1;

			if (ABS((player[closestplayer].ypos-obj->ypos)/65536)<64) 
			{
				if ((Distance<256*256) && (player[closestplayer].lastfire<20))
				{	//player shot, jump!
					obj->state=sJUMP;
					obj->phase=0;
					obj->counter=0;
					obj->curanim=AnimBase[mFENCER]+mFENCER_HOPIDLE;
					obj->ypos-=6*65536;
					obj->yspeed=-5*65536;
					obj->xspeed=obj->direction*2*65536;
					PlaySample(obj->xpos,obj->ypos,sCOMMON_UP,52,0);
				} 
				else
				if ((Distance<160*160) &&
					(level.difficulty>0) && 
					(obj->counter>AISPEED*2))
				{	//player didn't shoot, jump back slightly
					obj->state=sJUMP;
					obj->phase=0;
					obj->counter=0;
					obj->curanim=AnimBase[mFENCER]+mFENCER_HOPIDLE;
					obj->ypos-=4*65536;
					obj->yspeed=-2*65536;
					obj->xspeed=-obj->direction*1*65536;
					PlaySample(obj->xpos,obj->ypos,sCOMMON_UP,52,0);
				};
			}; //player on approx same Y pos
		}; //any player within radius?

		obj->counter++;
		if ((obj->counter&3)==3)
		{
			obj->phase++;
			if (obj->phase>=anims[obj->curanim].numframes)
				obj->phase=0;

			if (obj->phase==10)
			{
				switch ((RandFac(3))&3)
				{
				case 0:
					PlaySample(obj->xpos,obj->ypos,sCOMMON_FOEW1,0,0);
					break;
				case 1:
					PlaySample(obj->xpos,obj->ypos,sCOMMON_FOEW2,0,0);
					break;
				case 2:
					PlaySample(obj->xpos,obj->ypos,sCOMMON_FOEW3,0,0);
					break;
				default:
					PlaySample(obj->xpos,obj->ypos,sCOMMON_FOEW4,0,0);
					break;
				};
			};
		};

		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		DrawGameobj(num);
		break;

	case sJUMP:
		obj->xpos+=obj->xspeed;
		obj->ypos+=obj->yspeed;

		py=frames[obj->curframe].ty2+frames[obj->curframe].dy;
		hit=CheckPixel(obj->xpos,obj->ypos+py*65536);
		if (hit==0)
		{
			if (obj->ypos>level.waterlevel)
				obj->yspeed+=level.gravity/2;
			else
				obj->yspeed+=level.gravity*2;
			if (obj->yspeed>6*65536)
				obj->yspeed=6*65536;
		} else
		{
			obj->xspeed=0;
			obj->yspeed=0;
			obj->phase=0;
			obj->curanim=AnimBase[mFENCER]+mFENCER_STNDIDLE;
			obj->state=sSTILL;
			obj->counter=0;
		};


		obj->counter++;
		if (obj->counter>4)
		{
			obj->phase++;
			obj->counter=0;

			if (obj->phase>=anims[obj->curanim].numframes)
				obj->phase=anims[obj->curanim].numframes-1;
		};

		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		DrawGameobj(num);
		break;


	case sFREEZE:
		if (obj->freeze>0) obj->freeze--;
		if (obj->freeze==0) obj->state=obj->oldstate;

		if (obj->freeze<30)
		{
			oldy=obj->ypos;		obj->ypos+=4*Rand2Fac(16383);
			oldfreeze=obj->freeze;	obj->freeze=(Word8)RandFac(1); //let freeze anim flicker

			DrawGameobj(num);
			obj->freeze=oldfreeze;
            obj->ypos=oldy;
		} else
		DrawGameobj(num);

		break;

	case sKILL:
		cKILL(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;

	default:
		FAIL("fatchick(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //switch
} //fencer()



void demon(int num)
{
	Omonster *obj;
	int oldy;
	Word8 oldfreeze;
	int closestplayer,nump,dx;

	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		PutWalkerOnGround(num);
		obj->state=sIDLE;
		break;

	case sIDLE:
		obj->curanim=AnimBase[mDEMON]+mDEMON_IDLE;

		obj->phase=(Word8)((gamecounter/4)%(anims[obj->curanim].numframes));

		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		if (((gamecounter&3)==0) && (obj->phase==5))
			PlaySample(obj->xpos,obj->ypos,sCOMMON_HIBELL,0,0);

		DrawGameobj(num); //tho never freezed

		obj->counter++;

		if (obj->counter>AISPEED*2)
		{
			closestplayer=GetNearestPlayer(num,256*256);
			if (closestplayer>=0) 
			{
				if (player[closestplayer].xpos<obj->xpos)
				{
					obj->direction=-1;
				}
				else
				if (player[closestplayer].xpos>obj->xpos)
				{
					obj->direction=1;
				};

				if ((ABS((player[closestplayer].ypos-obj->ypos)/65536)<64))
				{
					obj->phase=0;
					obj->counter=0;
					obj->state=sATTACK;
					obj->curanim=AnimBase[mDEMON]+mDEMON_RUN1;
					PlaySample(obj->xpos,obj->ypos,sCOMMON_FOEW2,0,0);
				};

			};
		};
		break;

	case sATTACK:
		obj->counter++;
		if (obj->counter>4)
		{
			obj->phase++;
			obj->counter=0;

			if (obj->phase>=anims[obj->curanim].numframes)
			{
				obj->state=sWALK;
				obj->curanim=AnimBase[mDEMON]+mDEMON_RUNCYC;
				obj->counter=0;
				obj->phase=0;
				PlaySample(obj->xpos,obj->ypos,sDEMON_RUN,0,0);
			};
		};

		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		DrawGameobj(num);
		break;

	case sWALK:
		obj->xspeed=obj->direction*((2*65536+sintable((ABS(obj->phase-11)*32)&511)       )/2);

		cWALK(num);
		obj->counter++; 

		if (obj->counter>AISPEED*2)
		{
			nump=GetNearestPlayer(num,96*96);
			if (nump<0)
			{
				dx=player[nump].xpos-obj->xpos;
				if (((obj->direction<0) && (dx<0)) ||
					((obj->direction>0) && (dx>0)))
				{
					obj->state=sSTOP;
					obj->counter=0;
					obj->phase=0;
					obj->curanim=AnimBase[mDEMON]+mDEMON_STOP;
				};
			};
		};
		break;

	case sWAIT:
		obj->phase=(Word8)(gamecounter/8)%anims[obj->curanim].numframes;
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;

	case sSTOP:
		obj->xspeed=(obj->xspeed*7)/8;
		obj->xpos+=obj->xspeed; //brake!

		obj->counter++;
		if (obj->counter>4)
		{
			obj->phase++;
			obj->counter=0;

			if (obj->phase>=anims[obj->curanim].numframes)
			{
				obj->phase=0;
				obj->state=sIDLE;
				obj->curanim=AnimBase[mDEMON]+mDEMON_IDLE;
				obj->counter=0;
			};
		};

		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		DrawGameobj(num);
		break;

	case sFREEZE:
		if (obj->freeze>0) obj->freeze--;
		if (obj->freeze==0) obj->state=obj->oldstate;

		if (obj->freeze<30)
		{
			oldy=obj->ypos;		obj->ypos+=4*Rand2Fac(16383);
			oldfreeze=obj->freeze;	obj->freeze=(Word8)RandFac(1); //let freeze anim flicker

			DrawGameobj(num);
			obj->freeze=oldfreeze;
            obj->ypos=oldy;
		} else
		DrawGameobj(num);

		break;

	case sKILL:
		cKILL(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;

	default:
		FAIL("demon(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //switch
} //demon()



static void swarmingmoths(int num)
{
	Omonster *obj;
	int nump;
	int wantx,wanty;
	int oldy;
	int type;
	Word8 oldfreeze;

	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		PutObjectOnGround(num);

		type=GetEventParm(obj->xorg/(32*65536),obj->yorg/(32*65536),0,3);

		switch (type&3)
		{
		case 1:
			obj->curanim=AnimBase[mMOTH]+mMOTH_GREY;
			break;
		case 2:
			obj->curanim=AnimBase[mMOTH]+mMOTH_BLACK;
			break;
		case 3:
			obj->curanim=AnimBase[mMOTH]+mMOTH_PURPLE;
			break;
		default:
			obj->curanim=AnimBase[mMOTH]+mMOTH_RED;
			break;
		};//switch

		obj->state=sIDLE;
		obj->var1=obj->ypos;
		obj->counter=RandFac(1023);
		break;

	case sIDLE:
		
		obj->counter++;
		obj->phase=0;//(obj->counter/16)%anims[obj->curanim].numframes;
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);

		nump=GetNearestPlayer(num,64*64);
		if (nump>=0)
		{	
			obj->state=sFLY;
			obj->var2=64;//+(Distance/64);

			wantx=obj->xorg+sintable((obj->counter+obj->var2)*4)*32;
			wanty=obj->yorg+costable((obj->counter+obj->var2)*2)*24;

			obj->var3=(wantx-obj->xpos)/64;
			obj->var4=(wanty-obj->ypos)/64;

		};				
		break;

	case sFLY:
		if (obj->var3<0)
			obj->direction=-1;
		else
			obj->direction=1;

		obj->xpos+=obj->var3;
		obj->ypos+=obj->var4;
		obj->var2--;
		if (obj->var2<=0)
		{
			obj->state=sFLOAT;
		};
		obj->counter++;
		obj->phase=(obj->counter/6)%anims[obj->curanim].numframes;
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;

	case sFLOAT:
		obj->counter+=1+RandFac(3);

		obj->xpos=obj->xorg+(Rand2Fac(16383)+sintable(obj->counter*4))*32;
		if ((obj->counter&255)<128)
		{
			if (obj->direction>0)
				PlaySample(obj->xpos,obj->ypos,sCOMMON_FOEW1,10,30000+RandFac(2047));
			obj->direction=-1;
		}
		else
		{
			if (obj->direction<0)
				PlaySample(obj->xpos,obj->ypos,sCOMMON_FOEW1,10,30300+RandFac(2047));
			obj->direction=+1;
		};
		obj->ypos=obj->yorg+(Rand2Fac(16383)+costable(obj->counter*2))*24;

		obj->phase=(obj->counter/6)%anims[obj->curanim].numframes;
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);

		nump=GetNearestPlayer(num,128*128);
		if (nump<0)
		{	
			obj->state=sSTOP;
			obj->counter=0;

			obj->var2=64;
			obj->var3=(obj->xorg-obj->xpos)/64;
			obj->var4=(obj->var1-obj->ypos)/64;
		};
		break;

	case sSTOP:
		if (obj->var3<0)
			obj->direction=-1;
		else
			obj->direction=1;

		obj->xpos+=obj->var3;
		obj->ypos+=obj->var4;
		obj->var2--;
		if (obj->var2<=0)
			obj->state=sIDLE;

		obj->counter++;
		obj->phase=(obj->counter/6)%anims[obj->curanim].numframes;
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;

	case sFREEZE:
		if (obj->freeze>0) obj->freeze--;
		if (obj->freeze==0) 
		{
			obj->state=sSTOP;
			obj->counter=0;

			obj->var2=64;
			obj->var3=(obj->xorg-obj->xpos)/64;
			obj->var4=(obj->var1-obj->ypos)/64;
		};

		if (obj->freeze<30)
		{
			oldy=obj->ypos;		obj->ypos+=4*Rand2Fac(16383);
			oldfreeze=obj->freeze;	obj->freeze=(Word8)RandFac(1); //let freeze anim flicker

			DrawGameobj(num);
			obj->freeze=oldfreeze;
            obj->ypos=oldy;
		} else
		DrawGameobj(num);

		break;

	case sKILL:
		cKILL(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;

	default:
		FAIL("swarmingmoths(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //switch
} //swarmingmoths()


void moth(int num)
{
	Omonster *obj;
	int nump;
	int wantx,wanty;
	int oldy;
	int type;
	Word8 oldfreeze;

	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		PutObjectOnGround(num);

		type=GetEventParm(obj->xorg/(32*65536),obj->yorg/(32*65536),0,3);

		switch (type&3)
		{
		case 1:
			obj->curanim=AnimBase[mMOTH]+mMOTH_GREY;
			break;
		case 2:
			obj->curanim=AnimBase[mMOTH]+mMOTH_BLACK;
			break;
		case 3:
			obj->curanim=AnimBase[mMOTH]+mMOTH_PURPLE;
			break;
		default:
			obj->curanim=AnimBase[mMOTH]+mMOTH_RED;
			break;
		};//switch

		obj->state=sIDLE;
		obj->var1=obj->ypos;
		obj->counter=RandFac(1023);
		break;

	case sIDLE:
		
		obj->counter++;
		obj->phase=0;//(obj->counter/16)%anims[obj->curanim].numframes;
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);

		nump=GetNearestPlayer(num,64*64);
		if (nump>=0)
		{	
			obj->state=sFLY;
			obj->var2=64;//+(Distance/64);

			wantx=obj->xorg+sintable((obj->counter+obj->var2)*4)*32;
			wanty=obj->yorg+costable((obj->counter+obj->var2)*2)*24;

			obj->var3=(wantx-obj->xpos)/64;
			obj->var4=(wanty-obj->ypos)/64;

		};				
		break;

	case sFLY:
		if (obj->var3<0)
			obj->direction=-1;
		else
			obj->direction=1;

		obj->xpos+=obj->var3;
		obj->ypos+=obj->var4;
		obj->var2--;
		if (obj->var2<=0)
		{
			obj->state=sFLOAT;
		};
		obj->counter++;
		obj->phase=(obj->counter/6)%anims[obj->curanim].numframes;
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;

	case sFLOAT:
		obj->counter+=1+RandFac(3);

		obj->xpos=obj->xorg+(Rand2Fac(16383)+sintable(obj->counter*4))*32;
		if ((obj->counter&255)<128)
		{
			if (obj->direction>0)
				PlaySample(obj->xpos,obj->ypos,sCOMMON_FOEW1,10,30000+RandFac(2047));
			obj->direction=-1;
		}
		else
		{
			if (obj->direction<0)
				PlaySample(obj->xpos,obj->ypos,sCOMMON_FOEW1,10,30300+RandFac(2047));
			obj->direction=+1;
		};
		obj->ypos=obj->yorg+(Rand2Fac(16383)+costable(obj->counter*2))*24;

		obj->phase=(obj->counter/6)%anims[obj->curanim].numframes;
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);

		nump=GetNearestPlayer(num,128*128);
		if (nump<0)
		{	
			obj->state=sSTOP;
			obj->counter=0;

			obj->var2=64;
			obj->var3=(obj->xorg-obj->xpos)/64;
			obj->var4=(obj->var1-obj->ypos)/64;
		};
		break;

	case sSTOP:
		if (obj->var3<0)
			obj->direction=-1;
		else
			obj->direction=1;

		obj->xpos+=obj->var3;
		obj->ypos+=obj->var4;
		obj->var2--;
		if (obj->var2<=0)
			obj->state=sIDLE;

		obj->counter++;
		obj->phase=(obj->counter/6)%anims[obj->curanim].numframes;
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;

	case sFREEZE:
		if (obj->freeze>0) obj->freeze--;
		if (obj->freeze==0) 
		{
			obj->state=sSTOP;
			obj->counter=0;

			obj->var2=64;
			obj->var3=(obj->xorg-obj->xpos)/64;
			obj->var4=(obj->var1-obj->ypos)/64;
		};

		if (obj->freeze<30)
		{
			oldy=obj->ypos;		obj->ypos+=4*Rand2Fac(16383);
			oldfreeze=obj->freeze;	obj->freeze=(Word8)RandFac(1); //let freeze anim flicker

			DrawGameobj(num);
			obj->freeze=oldfreeze;
            obj->ypos=oldy;
		} else
		DrawGameobj(num);

		break;

	case sKILL:
		cKILL(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;

	default:
		FAIL("moth(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //switch
} //moth()



void dragfly(int num)
{
	Omonster *obj;
	int oldy;
	Word8 oldfreeze;
	int nump;
	int wantx,wanty;
	int wantsx,wantsy,maxspeed;


	obj=(Omonster *)(&gameobj[num]);


	if (obj->state!=sFREEZE)
		obj->channel=PlayLoopSample(obj->xpos,obj->ypos,sDRAGFLY_BEELOOP,43,22050,obj->channel);
	
	switch (obj->state)
	{
	case sSTART:
		obj->state=sIDLE;
		obj->counter=RandFac(255);
		break;

	case sIDLE:
		obj->phase=(Word8)((gamecounter/6)%anims[obj->curanim].numframes);
		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		obj->ypos+=sintable(gamecounter*16)/2;
		if (obj->ypos>level.waterlevel-12*65536) obj->ypos=level.waterlevel-12*65536;

		DrawGameobj(num);

		nump=GetNearestPlayer(num,128*128);
		if (nump>=0)
		{	
			if (player[nump].xpos<obj->xpos)
				obj->direction=-1;
			else
			if (player[nump].xpos>obj->xpos)
				obj->direction=1;

			obj->state=sFLY;
			obj->var2=64;

			wantx=player[nump].xpos+obj->direction*96*65536;
			wanty=player[nump].ypos-64*65536+sintable((gamecounter-obj->counter)*32)*4;

			obj->var3=(wantx-obj->xpos)/64;
			obj->var4=(wanty-obj->ypos)/64;
			obj->counter=0;
			obj->var1=nump;
		};				
		break;


	case sFLY:
		if (obj->var3<0)
			obj->direction=-1;
		else
			obj->direction=1;

		obj->xpos+=obj->var3;
		obj->ypos+=obj->var4;
		if (obj->ypos>level.waterlevel-12*65536) obj->ypos=level.waterlevel-12*65536;

		obj->var2--;
		if (obj->var2<=0)
		{
			obj->state=sFLOAT;
			obj->counter=8*AISPEED;
			obj->var3=RandFac(1);
			obj->var4=16+RandFac(63);
		};
		obj->phase=(Word8)((gamecounter/2)%anims[obj->curanim].numframes);
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;


	case sFLOAT:
		obj->var4--;
		if (obj->var4<0)
		{
			obj->var4=16+RandFac(63);
			obj->var3=RandFac(1);
		};

		obj->counter--;
		if (obj->counter>0)
		{
			if ((level.difficulty>1) && (obj->var3))
				wantx=player[obj->var1].xpos-player[obj->var1].direction*96*65536;//obj->direction*48*65536;
			else
				wantx=player[obj->var1].xpos-obj->direction*96*65536;//obj->direction*48*65536;
			wanty=player[obj->var1].ypos-64*65536+sintable((gamecounter-obj->counter)*32)*4;


			maxspeed=(2+level.difficulty)*65536;

			wantsx=wantx-obj->xpos;
			if (wantsx<-maxspeed) wantsx=-maxspeed;
			else
			if (wantsx>maxspeed) wantsx=maxspeed;

			wantsy=wanty-obj->ypos;
			if (wantsy<-maxspeed) wantsy=-maxspeed;
			else
			if (wantsy>maxspeed) wantsy=maxspeed;

			obj->xpos+=wantsx;
			obj->ypos+=wantsy;

			if (obj->xpos<player[obj->var1].xpos)
				obj->direction=1;
			else
			if (obj->xpos>player[obj->var1].xpos)
				obj->direction=-1;
		} else
		{
			obj->state=sATTACK;
			obj->counter=AISPEED*2;
		};

		obj->phase=(Word8)((gamecounter/2)%anims[obj->curanim].numframes);
		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		obj->ypos+=sintable(gamecounter*4);
		if (obj->ypos>level.waterlevel-12*65536) obj->ypos=level.waterlevel-12*65536;
		DrawGameobj(num);
		break;


	case sATTACK:
		wantsx=(player[obj->var1].xpos-obj->xpos);
		wantsy=(player[obj->var1].ypos-obj->ypos);

		maxspeed=(2+level.difficulty)*65536;

		if (wantsx>maxspeed)
			wantsx=maxspeed;
		else
		if (wantsx<-maxspeed)
			wantsx=-maxspeed;

		if (wantsy>maxspeed)
			wantsy=maxspeed;
		else
		if (wantsy<-maxspeed)
			wantsy=-maxspeed;

		if (obj->xspeed<wantsx)
			obj->xspeed+=1024*(1+level.difficulty);
		else
		if (obj->xspeed>wantsx)
			obj->xspeed-=1024*(1+level.difficulty);
		else
			obj->xspeed=wantsx;


		if (obj->yspeed<wantsy)
			obj->yspeed+=1024*(1+level.difficulty);
		else
		if (obj->yspeed>wantsy)
			obj->yspeed-=1024*(1+level.difficulty);
		else
			obj->yspeed=wantsy;

		obj->xpos+=obj->xspeed;
		obj->ypos+=obj->yspeed;		

		if (obj->ypos>level.waterlevel-12*65536) obj->ypos=level.waterlevel-12*65536;

		if (obj->xspeed<0) obj->direction=-1;
		else
		if (obj->xspeed>0) obj->direction=1;

		obj->phase=(Word8)((gamecounter/2)%anims[obj->curanim].numframes);
		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		DrawGameobj(num);

		obj->counter--;
		if (obj->counter<0)
		{
			obj->state=sSTOP;
			obj->counter=0;
		};
		break;


	case sSTOP:
		wantsx=(obj->xorg-obj->xpos);
		wantsy=(obj->yorg-obj->ypos);

		maxspeed=(4+level.difficulty)*65536;

		if (wantsx>maxspeed)
			wantsx=maxspeed;
		else
		if (wantsx<-maxspeed)
			wantsx=-maxspeed;

		if (wantsy>maxspeed)
			wantsy=maxspeed;
		else
		if (wantsy<-maxspeed)
			wantsy=-maxspeed;

		obj->xspeed=(obj->xspeed+wantsx)/2;
		obj->yspeed=(obj->yspeed+wantsy)/2;

		obj->xpos+=obj->xspeed;
		obj->ypos+=obj->yspeed;		
		if (obj->ypos>level.waterlevel-12*65536) obj->ypos=level.waterlevel-12*65536;

		if (obj->xspeed<-8000) obj->direction=-1;
		else
		if (obj->xspeed>8000) obj->direction=1;

		obj->phase=(Word8)((gamecounter/6)%anims[obj->curanim].numframes);
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);

		if (((obj->xpos/4096)==(obj->xorg/4096)) && ((obj->ypos/4096)==(obj->yorg/4096)))
		{
			obj->state=sIDLE;
			obj->counter=0;
		};
		break;

	case sFREEZE:
		if (obj->freeze>0) obj->freeze--;
		if (obj->freeze==0) obj->state=obj->oldstate;

		if (obj->freeze<30)
		{
			oldy=obj->ypos;		obj->ypos+=4*Rand2Fac(16383);
			if (obj->ypos>level.waterlevel-12*65536) obj->ypos=level.waterlevel-12*65536;

			oldfreeze=obj->freeze;	obj->freeze=(Word8)RandFac(1); //let freeze anim flicker

			DrawGameobj(num);
			obj->freeze=oldfreeze;
            obj->ypos=oldy;
		} else
		DrawGameobj(num);

		break;

	case sKILL:
		cKILL(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;

	default:
		FAIL("dragfly(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //switch
} //dragfly()






void bridge(int num)
{
	Omonster *obj;
	int bridge_len;
	int bridge_elements;
	int pressp[MAXPLAYERS]; 
	Word8 pressplayer[MAXPLAYERS];
	int hit;
	int t,tx,ty;
	int max,amp,fase;
	int nump;
	int thisx;
	int leftx,rightx;
	int leftamp,rightamp;


//first, check collision with bridge

	obj=(Omonster *)(&gameobj[num]);

	if (obj->state==sSTART)
	{
		obj->state=sSTILL;

		obj->var1=32*GetEventParm(obj->xpos/(32*65536),obj->ypos/(32*65536),0,4);

		t=GetEventParm(obj->xpos/(32*65536),obj->ypos/(32*65536),4,3);
		if (t==7) t=0;
		obj->curanim=AnimBase[mBRIDGE]+mBRIDGE_ELEM1+t; //hacked, but are always behind eachother


		t=GetEventParm(obj->xpos/(32*65536),obj->ypos/(32*65536),7,4);
		if (t==0) t=4;
		obj->var2=obj->var1/t;

		obj->var3=obj->var1+1;

		obj->xpos-=16*65536;

//		PutObjectOnGround(num);
		obj->ypos-=6*65536;		
	}


	hit=0;
	for (t=0;t<MAXPLAYERS;t++)
	if (player[t].Active)
	{
		tx=(player[t].xpos-obj->xpos)/65536;
		ty=(player[t].ypos-obj->ypos)/65536;

		if ((tx>=0) && (tx<=obj->var1) &&
			(ty>-24) && (ty<obj->var2) &&
			(player[t].yspeed>-65536))

		{
			pressp[hit]=tx; //xposition on bridge
			pressplayer[hit]=t;

			hit++;
		} else
		{
			if (player[t].platform==num)
			{
				player[t].platform=0;
				player[t].platformtype=0;
				player[t].movexspeed=0;
				player[t].moveyspeed=0;
			}
		}
	}

/*
	if (obj->state==sFREEZE)
	{
		if (obj->freeze>0) 
		{
			obj->freeze--;
		};
		if (obj->freeze<=0)
			obj->state=obj->oldstate; //whatever
	}

	if (obj->state==sFREEZE)
	{
		for (t=0;t<=bridge_elements;t++)
		{
			AddSprite(obj->xpos+t*bridge_el_width*65536,obj->ypos,SPRITELAYER,obj->direction,spriteFREEZE,0,obj->curframe);
		};


		bridge_len=t=bridge_elements=0;
		do {

			obj->curframe=anims[obj->curanim].framelist[t];

			AddSprite(obj->xpos+bridge_len*65536,obj->ypos,SPRITELAYER,obj->direction,spriteFREEZE,0,obj->curframe);

			bridge_elements++;

			bridge_len+=frames[obj->curframe].dx;

			t++;
			if (t>=anims[obj->curanim].numframes)
				t=0;
		} while (bridge_len<obj->var1);



		if (hit)
		for (t=0;t<hit;t++)
		{
			el=pressp[t];
			nump=pressplayer[t];

			player[nump].ypos=obj->ypos-24*65536;
			player[nump].platform=num;
			player[nump].platformtype=1;
			if (player[nump].downattack<DOWNATTACKLEN)
				player[nump].downattack=DOWNATTACKLEN;
		}
	}
	else
*/	if (hit==0)
	{ //straight bridge, or terugveren

		bridge_len=0;
		t=0;
		bridge_elements=0;
		do {

			obj->curframe=anims[obj->curanim].framelist[t];

			thisx=obj->xpos+(bridge_len-frames[obj->curframe].tx2)*65536;

			AddSprite(thisx,obj->ypos,SPRITELAYER,obj->direction,spriteNORMAL,0,obj->curframe);

			bridge_elements++;


			bridge_len+=frames[obj->curframe].dx;

			t++;
			if (t>=anims[obj->curanim].numframes)
				t=0;
		} while (bridge_len<obj->var1);

		obj->var3=bridge_len;
	}
	else
	if (hit==1)
	{ //single bridge 

		fase=(512*pressp[0])/obj->var1;
		max=(obj->var2*sintable(fase));

		nump=pressplayer[0];
		player[nump].ypos=obj->ypos+max-24*65536;
		player[nump].platform=num;
		player[nump].platformtype=1;
		player[nump].movexspeed=0; //no extra speed here
		player[nump].moveyspeed=0;
		if (player[nump].downattack<DOWNATTACKLEN)
			player[nump].downattack=DOWNATTACKLEN;




		max/=65536;
		bridge_len=t=bridge_elements=0;
		do {
			if (bridge_len<pressp[0])	//left
			{
				amp=obj->ypos+(max*sintable((256*bridge_len)/pressp[0]));
			}
			else						//rightside
			{
				amp=obj->ypos+(max*costable( (256*(bridge_len-pressp[0]))/(obj->var3-pressp[0]) ));
			}


			obj->curframe=anims[obj->curanim].framelist[t];

			thisx=obj->xpos+(bridge_len-frames[obj->curframe].tx2)*65536;

			AddSprite(thisx,amp,SPRITELAYER,obj->direction,spriteNORMAL,0,obj->curframe);

			bridge_elements++;

			bridge_len+=frames[obj->curframe].dx;

			t++;
			if (t>=anims[obj->curanim].numframes)
				t=0;
		} while (bridge_len<obj->var1);
	}
	else
	{	//multiplayer bridge 

		t=0;
		leftx=12312312;
		rightx=0;
		do {
			if (pressp[t]<leftx) 
				leftx=pressp[t];

			if (pressp[t]>rightx)
				rightx=pressp[t];

		} while (++t<hit);


		fase=(512*leftx)/obj->var1;
		max=(obj->var2*sintable(fase));
		leftamp=max;

		fase=(512*rightx)/obj->var1;
		max=(obj->var2*sintable(fase));
		rightamp=max;


		t=0;
		do {
			if ((pressp[t]>leftx) && (pressp[t]<rightx))
			{
				amp=obj->ypos+leftamp+(rightamp-leftamp)*(pressp[t]-leftx)/(rightx-leftx);
			}
			else
			{
				fase=(512*pressp[t])/obj->var1;
				max=(obj->var2*sintable(fase));
				amp=obj->ypos+max;
			}

			nump=pressplayer[t];
			player[nump].ypos=amp-24*65536;
			player[nump].platform=num;
			player[nump].platformtype=1;
			player[nump].movexspeed=0; //no extra speed here
			player[nump].moveyspeed=0;
			if (player[nump].downattack<DOWNATTACKLEN)
				player[nump].downattack=DOWNATTACKLEN;

		} while (++t<hit);


		bridge_len=t=bridge_elements=0;
		do {

			if (bridge_len<leftx)	//left
			{
				amp=obj->ypos+((leftamp/65536)*sintable((256*bridge_len)/leftx));
			}
			else
			if (bridge_len>rightx) //rightside
			{
				amp=obj->ypos+((rightamp/65536)*costable( (256*(bridge_len-rightx))/(obj->var3-rightx) ));
			}
			else
//			if ((bridge_len>leftx) && (bridge_len<rightx))
			{
				amp=obj->ypos+leftamp+(rightamp-leftamp)*(bridge_len-leftx)/(rightx-leftx);
			}

			obj->curframe=anims[obj->curanim].framelist[t];

			thisx=obj->xpos+(bridge_len-frames[obj->curframe].tx2)*65536;

			AddSprite(thisx,amp,SPRITELAYER,obj->direction,spriteNORMAL,0,obj->curframe);

			bridge_elements++;

			bridge_len+=frames[obj->curframe].dx;

			t++;
			if (t>=anims[obj->curanim].numframes)
				t=0;
		} while (bridge_len<obj->var1);
	}


} //bridge()








void floatlizard(int num)
{
	Omonster *obj;
	int anim,curframe,fase;
	int nump;
	int cobj;
	Omonster *obj2;
	Tframes *frame;
	int rx,ry;
	int oldy;
	Word8 oldfreeze;
	int dx,dy;
	int maxspeed;
	int px,py,hit1,hit2;

	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		if (obj->creator!=LEVELSLOT)
		{
			obj->var5=4*AISPEED;
			obj->var6=1;
		}
		else
		{	//duration of flying when helicopter is seperate!
			obj->var5=AISPEED*GetEventParm(obj->xpos/(32*65536),obj->ypos/(32*65536),0,8);
			if (obj->var5==0)
				obj->var5=4*AISPEED;

			obj->var6=GetEventParm(obj->xpos/(32*65536),obj->ypos/(32*65536),8,1);
		};

		obj->state=sSLEEP;

		//obj->var3=1;	//flying!
		break;

	case sSLEEP:
		cSLEEP(num,192*192);
		anim=AnimBase[mLIZARD]+mLIZARD_JETPACK;
		fase=(gamecounter/3)%(anims[anim].numframes);
		curframe=anims[anim].framelist[fase];

		AddSprite(obj->xpos,obj->ypos+4*65536,SPRITELAYER,
					obj->direction,spriteNORMAL,0,curframe);
//lizard
		obj->curanim=AnimBase[mLIZARD]+mLIZARD_FLY;
		obj->phase=(Word8)(gamecounter/10)%(anims[obj->curanim].numframes);
		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		DrawGameobj(num);
		break;

	case sWAKE:
		obj->state=sFLY;
		obj->curanim=AnimBase[mLIZARD]+mLIZARD_FLY;
	case sFLY:
		dx=(obj->xpos-player[obj->var1].xpos)/65536;
		dy=(obj->ypos-player[obj->var1].ypos)/65536;

		if (dx>100)
			obj->xacc=-2048;
		else
		if (dx<-100)
			obj->xacc=2048;
		else
			obj->xacc=0;

		if (obj->xacc)
			obj->xspeed+=obj->xacc;
		else
			obj->xspeed=(obj->xspeed*31)/32;

		if (dy>0)
			obj->yacc=-2048;
		else
		if (dy<-80)
			obj->yacc=2048;
		else
			obj->yacc=0;

		if (obj->yacc)
			obj->yspeed+=obj->yacc;
		else
			obj->yspeed=(obj->yspeed*31)/32;

		maxspeed=(2+2*level.difficulty)*65536;
		if (obj->xspeed>maxspeed)
			obj->xspeed=maxspeed;
		else
		if (obj->xspeed<-maxspeed)
			obj->xspeed=-maxspeed;

		if (obj->yspeed>maxspeed)
			obj->yspeed=maxspeed;
		else
		if (obj->yspeed<-maxspeed)
			obj->yspeed=-maxspeed;

		obj->xpos+=obj->xspeed;
		obj->ypos+=obj->yspeed;


//jetpack
		anim=AnimBase[mLIZARD]+mLIZARD_JETPACK;
		fase=(gamecounter/3)%(anims[anim].numframes);
		curframe=anims[anim].framelist[fase];

		AddSprite(obj->xpos,obj->ypos+4*65536,SPRITELAYER,
					obj->direction,spriteNORMAL,0,curframe);
//lizard
		anim=AnimBase[mLIZARD]+mLIZARD_FLY;
		fase=(gamecounter/10)%(anims[anim].numframes);
		curframe=anims[anim].framelist[fase];

		AddSprite(obj->xpos,obj->ypos,SPRITELAYER,
					obj->direction,spriteNORMAL,0,curframe);

		nump=GetNearestPlayer(num,128*128);
		if (nump>=0)
		{	
			if (player[nump].xpos<obj->xpos)
				obj->direction=-1;
			else
			if (player[nump].xpos>obj->xpos)
				obj->direction=1;

			obj->var2++;

			if (obj->var2>AISPEED)
			{
				obj->state=sATTACK;

				obj->counter=0;
				obj->phase=0;
				obj->curanim=AnimBase[mLIZARD]+mLIZARD_ATTACK;
				obj->var1=nump;
			};
		};				
		break;


	case sATTACK:

		obj->counter++;
		if (obj->counter>4)
		{
			obj->phase++;
			obj->counter=0;
			if (obj->phase>=anims[obj->curanim].numframes)
			{
				obj->curanim=AnimBase[mLIZARD]+mLIZARD_FLY;
				obj->phase=0;
				obj->state=sFLY;
				obj->var2=0;
				obj->counter=0;
			};

			if (obj->phase==12)
			{	//throw bomb
				frame=&frames[anims[obj->curanim].framelist[12]];

				if (obj->direction<0)
					rx=obj->xpos-((frame->tx2-frame->tx3)*65536);
				else
	   				rx=obj->xpos+((frame->tx2-frame->tx3)*65536);

				ry=obj->ypos+((frame->ty2-frame->ty3)*65536);

				cobj=AddObject(rx,ry,aBOMB,0);
				if (cobj>0)
				{
					obj2=(Omonster *)(&gameobj[cobj]);
					obj2->curanim=AnimBase[mLIZARD]+mLIZARD_BOMBROT;

					obj2->xspeed=obj->direction*65536;

					obj2->yspeed=-4*32768;
					obj2->state=sFLY;
				};
			};
		};

//jetpack
		anim=AnimBase[mLIZARD]+mLIZARD_JETPACK;
		fase=(gamecounter/3)%(anims[anim].numframes);
		curframe=anims[anim].framelist[fase];

		AddSprite(obj->xpos,obj->ypos+4*65536,SPRITELAYER,
					obj->direction,spriteNORMAL,0,curframe);
//lizard
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;

	case sFALL:
/*		if (obj->counter==0)
		{
			obj->curanim=AnimBase[mLIZARD]+mLIZARD_FLY;

			cobj=AddObject(obj->xpos,obj->ypos+4*65536,aCOPTER,0);
			if (cobj>0)
			{
				obj2=(Omonster *)(&gameobj[cobj]);
				obj2->direction=obj->direction;
				obj2->state=sSTART;
				obj2->var5=obj->var5;
			};
		};
*/		obj->counter++;

		obj->xpos+=obj->xspeed;
		obj->ypos+=obj->yspeed;

		if (obj->xspeed<0)
			px=-frames[obj->curframe].tx2-(frames[obj->curframe].dx/2);
		else
			px=(frames[obj->curframe].dx/2)+frames[obj->curframe].tx2;

		py=frames[obj->curframe].ty2+frames[obj->curframe].dy;

		hit1=CheckPixel(obj->xpos+(px*65536),obj->ypos+(py*65536));

		if (hit1==0)
		{
			if (obj->ypos>level.waterlevel)
				obj->yspeed+=level.gravity/4;
			else
				obj->yspeed+=level.gravity;
		}
		else
		if (hit1)
		{	//initial falling down
			obj->yacc=0;
			obj->yspeed=0;
			obj->state=sWALK;
			obj->curanim=AnimBase[mLIZARD]+mLIZARD_WALK;
			obj->xspeed=obj->direction*65536;
			obj->energy=1;
			obj->xacc=0;
		};

	//CHECKING FOR BOUNCING OF WALLS OR RAVINES

		if (obj->xspeed<0)
			px=-frames[obj->curframe].tx2-frames[obj->curframe].dx;
		else
			px=frames[obj->curframe].dx+frames[obj->curframe].tx2;

		py=frames[obj->curframe].ty2+frames[obj->curframe].dy;

		hit1=CheckPixel(obj->xpos+(px*65536),obj->ypos+(py<<16));

		hit2=CheckPixel(obj->xpos+(px*65536),obj->ypos+((py-16)<<16));

		if ((hit2==1) || (hit1==0) || (tileattr==areaSTOPENEMY))	//bounce of wall
		{
			obj->xspeed=-obj->xspeed;
			obj->xpos+=obj->xspeed;
		};
/*
		if (obj->xspeed<0)
			obj->direction=-1;
		else
			obj->direction=1;
*/
		obj->phase=(Word8)((gamecounter/8)%(anims[obj->curanim].numframes));

		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		DrawGameobj(num);
		break;

	case sWALK:
		obj->xspeed=obj->direction*((65536+sintable((ABS(obj->phase-7)*32)&511) )/2);

		if (((gamecounter&7)==0) && (obj->phase==0))
		{
			PlaySample(obj->xpos,obj->ypos,LIZARDSOUNDS[RandFac(3)],0,0);
		}

		cWALK(num);
		break;

	case sWAIT:
		obj->phase=(Word8)(gamecounter/8)%anims[obj->curanim].numframes;
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;

	case sFREEZE:
		if (obj->freeze>0) obj->freeze--;
		if (obj->freeze==0) obj->state=obj->oldstate;

		if ((obj->oldstate==sSLEEP) ||
			(obj->oldstate==sWAKE) ||
			(obj->oldstate==sFLY) ||
			(obj->oldstate==sATTACK))
		{
			anim=AnimBase[mLIZARD]+mLIZARD_JETPACK;
			curframe=anims[anim].framelist[0];

			AddSprite(obj->xpos,obj->ypos+4*65536,SPRITELAYER,
						obj->direction,spriteFREEZE,0,curframe);
		}	//draw frozen jetpack!

		if (obj->freeze<30)
		{
			oldy=obj->ypos;		obj->ypos+=4*Rand2Fac(16383);
			oldfreeze=obj->freeze;	obj->freeze=(Word8)RandFac(1); //let freeze anim flicker

			DrawGameobj(num);
			obj->freeze=oldfreeze;
            obj->ypos=oldy;
		} else
			DrawGameobj(num);
		break;

	case sKILL:
		cKILL(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;

	default:
		FAIL("floatlizard(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //switch

} //floatlizard





void netcopter(int num)
{
	Omonster *obj;
	Word8 data[64];
	TactPACKETS *action;

	if (MultiPlayerConnection>gameLOCAL) {
		obj=(Omonster *)(&gameobj[num]);
		
		action=(TactPACKETS *)&data[0];
		action->GENERIC.PacketID=actCOPTERSTATE;
		action->COPTERSTATE.UIDLow=UID[num];
		action->COPTERSTATE.UIDHigh=UID[num]>>8;

		if (obj->var1>=PLAYSLOT)
		{
			action->COPTERSTATE.nump=obj->var1-PLAYSLOT;
			action->COPTERSTATE.fly=1;
		}
		else
		{
			action->COPTERSTATE.nump=0;
			action->COPTERSTATE.fly=0;
		}

		if (obj->state==sDONE)
			action->COPTERSTATE.state=0;
		else
		if (obj->state==sKILL)
			action->COPTERSTATE.state=1;
		else
		if (obj->state==sEXTRA)
			action->COPTERSTATE.state=2;
		else
			action->COPTERSTATE.state=3;

		if (!LocalComputerIsServer)
			Client_SendActions(action);
		else
			Server_SendActions(action,0);	//send to all!
	}

}	//netcopter



void copter(int num)
{
	Omonster *obj;


	obj=(Omonster *)(&gameobj[num]);

	if ((obj->var1>=PLAYSLOT) && (obj->state!=sEXTRA))
	{	//clear it!!!
		player[obj->var1-PLAYSLOT].fly=0;
		obj->var1=0;
	}

	switch (obj->state)
	{
	case sSTART:
		if (obj->creator!=LEVELSLOT)
		{
			obj->var5=4*AISPEED;
		}
		else
		{	//duration of flying when helicopter is seperate!
			obj->var5=AISPEED*GetEventParm(obj->xpos/(32*65536),obj->ypos/(32*65536),0,8);
		};

		if (obj->var5<=0)
			obj->var5=4*AISPEED;
		else
		if (obj->var5>9*AISPEED)
			obj->var5=9*AISPEED;

//		obj->xspeed=0;
//		obj->yspeed=0;
		obj->var4=obj->var5;
		obj->state=sFLY;
//		obj->counter=0;
		break; 

	case sFLY:
	case sDONE:

		obj->counter++;

//		obj->xspeed+=obj->direction*8192;//32768;

		obj->xpos+=obj->xspeed;
		obj->ypos+=obj->yspeed;

		obj->channel=PlayLoopSample(obj->xpos,obj->ypos,sPICKUPS_HELI2,40,0,obj->channel);

		if (obj->counter>AISPEED*4)
		{
			obj->state=sKILL;
			netcopter(num);
		}
		
		obj->phase=(Word8)((gamecounter/3)%(anims[obj->curanim].numframes));
		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		if (obj->counter>AISPEED*3)
		{
			if (framecounter&1)
				DrawGameobj(num);
		} else
			DrawGameobj(num);
		break;

	case sEXTRA:

		obj->channel=PlayLoopSample(obj->xpos,obj->ypos,sPICKUPS_HELI2,40,0,obj->channel);

		if (obj->var1<PLAYSLOT)
		{
			DrawGameobj(num);
			obj->state=sFLY;
			return;
		}
		obj->counter++;

		if ((obj->var6) && (player[obj->var1-PLAYSLOT].lastjump>0))
			obj->var6--;

		if ((obj->counter>obj->var4) ||
			((obj->var6==0) && (player[obj->var1-PLAYSLOT].lastjump==0)))
		{
			player[obj->var1-PLAYSLOT].fly=0;
			obj->var1=0;
			obj->state=sDONE;
			obj->yspeed=-16384;
			DrawGameobj(num);
			netcopter(num);
			return;
		}

		obj->xpos=player[obj->var1-PLAYSLOT].xpos;
		obj->ypos=player[obj->var1-PLAYSLOT].ypos;
		obj->phase=(Word8)((obj->counter/3)%(anims[obj->curanim].numframes));
		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		if (obj->counter>obj->var4-AISPEED)
		{
			if (framecounter&1)
				DrawGameobj(num);
		}
		else
			DrawGameobj(num);
		break;


	case sFREEZE:
		if (obj->freeze>0) obj->freeze--;
		if (obj->freeze==0) obj->state=obj->oldstate;
		DrawGameobj(num);
		break;

	case sKILL:
		cKILL(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;

	default:
		FAIL("copter(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //obj->state
} //copter()






void monkey(int num)
{
	Omonster *obj;
	int nump;
	int hit1,hit2,px,py;
	int res;
	Obullets *bobj;

	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		if (obj->var4==1)
		{
			obj->state=sIDLE;
			obj->var1=2;
		}
		else
		{
			obj->state=sWALK;
		}

		PutWalkerOnGround(num);
		break;


	case sWALK:
		obj->counter++;
		obj->xspeed=obj->direction*((65536+sintable((ABS(obj->phase-7)*32)&511) )/2);

		obj->xpos+=obj->xspeed;
		obj->ypos+=obj->yspeed;

		if (obj->xspeed>6*65536) obj->xspeed=6*65536;
		else
		if (obj->xspeed<-6*65536) obj->xspeed=-6*65536;

	//CHECKING FOR BOUNCING OF WALLS OR RAVINES
		if (obj->xspeed<0)
			px=-frames[obj->curframe].tx2-frames[obj->curframe].dx;
		else
			px=frames[obj->curframe].dx+frames[obj->curframe].tx2;

		py=frames[obj->curframe].ty2-frames[obj->curframe].ty;

		hit1=CheckPixel(obj->xpos+(px*65536),obj->ypos+(py<<16));

		hit2=CheckPixel(obj->xpos+(px*65536),obj->ypos+((py-16)<<16));

		if ((hit2==1) || (hit1==0) || (tileattr==areaSTOPENEMY))	//bounce of wall
		{
			if (obj->xspeed>=0)
				px=-frames[obj->curframe].tx2-frames[obj->curframe].dx;
			else
				px=frames[obj->curframe].dx+frames[obj->curframe].tx2;

			hit2=CheckPixel(obj->xpos+(px*65536),obj->ypos+((py-16)<<16));

			if (hit2==1)	//dont do the direction swap
			{
				obj->state=sWAIT;
			}
			else
			{
				obj->xspeed=-obj->xspeed;
				obj->xpos+=obj->xspeed;
			}
		}

		if (obj->xspeed<0)
			obj->direction=-1;
		else
			obj->direction=1;

		//check other objects, if not/.... TO DO

		if ((obj->counter&7)==0)
		{
			obj->phase++;
			if (obj->phase>=anims[obj->curanim].numframes)
				obj->phase=0;
		}
		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		DrawGameobj(num);


		nump=GetNearestPlayer(num,256*256);
		
		if (nump>=0)
		{
			if (player[nump].xpos<obj->xpos)
				obj->direction=-1;
			else
			if (player[nump].xpos>obj->xpos)
				obj->direction=1;

			if (Distance<256*256)
			{
				obj->var6=player[nump].xpos;//nump;
				obj->state=sEXTRA;
				obj->curanim=AnimBase[mMONKEY]+mMONKEY_STOP;
				obj->phase=0;
				obj->counter=0;
			}	
		}
		else
		if (obj->counter>AISPEED*4)
		{
			obj->state=sIDLE;
			obj->curanim=AnimBase[mMONKEY]+mMONKEY_STOP;
			obj->phase=0;
			obj->counter=0;
		}
		break;


	case sWAIT:
		if ((gamecounter&7)==0)
			obj->phase++;
		if (obj->phase>=anims[obj->curanim].numframes)
			obj->phase=0;
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;


	case sIDLE:
		if (obj->var1==0)
		{
			obj->curanim=AnimBase[mMONKEY]+mMONKEY_STOP;
			obj->counter++;
			if (obj->counter>4)
			{
				obj->phase++;
				obj->counter=0;
				if (obj->phase>=anims[obj->curanim].numframes)
				{
					obj->phase=0;
					obj->curanim=AnimBase[mMONKEY]+mMONKEY_IDLE;
					obj->var1=2;
				}
			}
		}
		else
		if (obj->var1==1)
		{
			obj->curanim=AnimBase[mMONKEY]+mMONKEY_START;
			obj->counter++;
			if (obj->counter>4)
			{
				obj->phase++;
				obj->counter=0;
				if (obj->phase>=anims[obj->curanim].numframes)
				{
					nump=GetNearestPlayer(num,256*256);//100*100);
					if (nump>=0)
					{
						obj->var6=player[nump].xpos;//nump;
						obj->state=sEXTRA;
						obj->curanim=AnimBase[mMONKEY]+mMONKEY_STOP;
						obj->phase=0;
						obj->counter=0;
					}
					else
					{
						obj->phase=0;
						obj->curanim=AnimBase[mMONKEY]+mMONKEY_WALK;
						obj->state=sWALK;
					}
				}
			}
		}
		else
		{

			obj->var5++;
			if (obj->var5>5)
			{
				obj->var5=0;
				obj->phase++;
			}
			if (obj->phase>=anims[obj->curanim].numframes)
				obj->phase=0;

			
			obj->counter++;

			if (obj->var4==0) 
			{
				if (obj->counter>AISPEED)
				{
					obj->counter=0;
					obj->var1=1;
				}
			}
			else
			{
				if (obj->counter>(3-level.difficulty)*AISPEED)
				{
					nump=GetNearestPlayer(num,256*256);
					if (nump>=0)
					{
						obj->var6=player[nump].xpos;//nump;
						if (player[nump].xpos<obj->xpos)
							obj->direction=-1;
						else
						if (player[nump].xpos>obj->xpos)
							obj->direction=1;

						obj->var2=Distance;
						obj->state=sATTACK;
						obj->counter=0;
						obj->curanim=AnimBase[mMONKEY]+mMONKEY_THROW;
						obj->phase=0;
					}
				}
			}
		}

		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;


	case sEXTRA:
		obj->counter++;
		if (obj->counter>4)
		{
			obj->phase++;
			obj->counter=0;
			if (obj->phase>=anims[obj->curanim].numframes)
			{
				obj->phase=0;
				obj->curanim=AnimBase[mMONKEY]+mMONKEY_THROW;
				obj->state=sATTACK;
			}
		}

		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;

	case sATTACK:

		obj->counter++;
		if (obj->counter>4)
		{
			obj->phase++;
			obj->counter=0;

			if (obj->phase==8)
			{
				res=AddBullet(num,aBULLET);

				if (res>=0)
				{	//BANANABUL
					bobj=(Obullets *)(&gameobj[res]);
					bobj->ProcPtr=&cBANANA;

					bobj->xspeed=(obj->var6-bobj->xpos)/32;
					//bobj->xspeed=(player[obj->var6].xpos-bobj->xpos)/32;
					//bobj->yspeed=(player[obj->var6].ypos+16*65536-bobj->ypos)/32;
					bobj->state=sFLY;
					bobj->direction=obj->direction;
					bobj->xspeed+=bobj->direction*8192; //extra little speed

					bobj->force=1;
					bobj->objtype=oENEMYBULLET;
					bobj->curanim=AnimBase[mMONKEY]+mMONKEY_BANANA;
					bobj->killanim=AnimBase[mMONKEY]+mMONKEY_BANASPLT;
					bobj->lifetime=AISPEED/2;
					bobj->lighttype=2;
				};


			}
			else
			if (obj->phase>=anims[obj->curanim].numframes)
			{
				obj->curanim=AnimBase[mMONKEY]+mMONKEY_IDLE;
				obj->phase=0;
				obj->state=sIDLE;
				if (obj->var4==0) obj->var1=1;
				obj->counter=0;
			}
		}

		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;


	case sFREEZE:
		if (obj->freeze>0) obj->freeze--;
		if (obj->freeze==0) obj->state=obj->oldstate;
		DrawGameobj(num);
		break;

	case sKILL:
		cKILL(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;

	default:
		FAIL("monkey(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}	//obj->state
} //monkey()




void cDESTRUCTSCENERY(int num)
{
	Omonster *obj;
	int x,y,tile,calc;
//#ifdef _DEBUGSTATE
//	char s[10];
//#endif


	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		x=obj->xorg/(32*65536);
		y=obj->yorg/(32*65536);
		
		tile=GetSpriteLayerTile(x,y);

		obj->var1=tile;	//org animtile
		obj->var2=GetDestructAnimTileFrames(tile)-1;
		obj->var3=GetDestructAnimTile(tile,0);
		PutSpriteLayerTile(x, y, (Word16)obj->var3);

		obj->energy=(Word8)obj->var2;
		obj->phase=0;

		obj->nohit=3;

		obj->curframe=anims[obj->curanim].framelist[0]; //collision checking!
		obj->state=sSLEEP;
		obj->xpos+=65536;

		if (obj->load==aCOLLAPSESCENERY)
		{
			obj->ypos-=8*65536;
			obj->var4=AISPEED*GetEventParm(x,y,0,10)/4;
		}
		else
		if (obj->load==aSTOMPSCENERY)
		{
			obj->curanim=AnimBase[mDESTSCEN]+mDESTSCEN_64;
			obj->curframe=anims[obj->curanim].framelist[0]; //collision checking!
			//obj->ypos-=10*65536; //trigger differently!
		}
		else
		{
			x=obj->xorg/(32*65536);
			y=obj->yorg/(32*65536);
			calc=GetEventParm(x,y,10,5); //speed dest

			if (calc) //wider: better collision 
				obj->curanim=AnimBase[mDESTSCEN]+mDESTSCEN_48;
			else
				obj->curanim=AnimBase[mDESTSCEN]+mDESTSCEN_48BULLET;

			obj->curframe=anims[obj->curanim].framelist[0]; //collision checking!
			obj->ypos+=65536;
			obj->xpos-=65536;
		
		}

		//obj->var5 : this tile has changed (if not, we can deactivate it)
		//            this to eat up less object space
		break;

	case sFREEZE:
		if (obj->freeze>0) obj->freeze--;
        if (obj->freeze==0) obj->state=obj->oldstate;
		else
		if (((obj->freeze<32) && ((framecounter&3)<2)) || (obj->freeze>32))
		{
			calc=anims[AnimBase[mDESTSCEN]+mDESTSCEN_ICED].framelist[0];
			
			AddSprite(obj->xorg,obj->yorg,SPRITELAYER,0,spriteNORMAL,0,calc);
		}
		break;


	case sSLEEP:
		//obj->curframe=anims[AnimBase[mDESTSCEN]+mDESTSCEN_32].framelist[0];
		//already set!

		if ((obj->load==aCOLLAPSESCENERY) && (obj->counter))
		{	//COLLAPSING!!!
			obj->counter++;
			if (obj->counter>obj->var4)
				obj->state=sEXTRA;
		}
		break;


	case sHIT:		//hit, show next tile in anim
		obj->phase=obj->var2-obj->energy;
		tile=GetDestructAnimTile(obj->var1,obj->phase);
		x=obj->xorg/(32*65536);
		y=obj->yorg/(32*65536);

		PutSpriteLayerTile(x, y, (Word16)tile);
		obj->state=sSLEEP;
		obj->var5++;
		break;


	case sEXTRA:
		x=obj->xorg/(32*65536);
		y=obj->yorg/(32*65536);

		calc=GetEventParm(x,y,10,5);
		obj->phase=0;

		if (calc==0)
			obj->var4=7;
		else
			obj->var4=AISPEED/calc;

		obj->counter=obj->var4;
			PlaySample(obj->xpos,obj->ypos,sCOMMON_COLLAPS,0,0);


		obj->state=sFALL;
		//break; //fallthru

	case sFALL:
		obj->counter++;
		if (obj->counter>obj->var4)
		{
			obj->counter=0;
			obj->phase++;
			if (obj->phase>=obj->var2)
			{
				obj->state=sKILL;
				obj->phase=(Word8)obj->var2;
			}
		}

		if (obj->state==sKILL)
		{
			x=obj->xorg/(32*65536);
			y=obj->yorg/(32*65536);
			tile=GetDestructAnimTile(obj->var1,obj->var2+1);
			AddParticleTile(x,y,tile,1);
			//PlaySample(obj->xpos,obj->ypos,sCOMMON_DAMPED1,128,0);
//			PlaySample(obj->xpos,obj->ypos,sCOMMON_COLLAPS,0,0);

		
			obj->nohit=3+8;
			obj->freeze=0;
			obj->state=sDONE;
			obj->curframe=0;
			obj->counter=AISPEED*30;
		}

		tile=GetDestructAnimTile(obj->var1,obj->phase);
		x=obj->xorg/(32*65536);
		y=obj->yorg/(32*65536);
		PutSpriteLayerTile(x, y, (Word16)tile);
		obj->var5++;
		break;


	case sKILL:
		x=obj->xorg/(32*65536);
		y=obj->yorg/(32*65536);
		tile=GetDestructAnimTile(obj->var1,obj->var2+1);
 		AddParticleTile(x,y,tile,0);
		PlaySample(obj->xpos,obj->ypos,sCOMMON_DAMPED1,128,0);

		tile=GetDestructAnimTile(obj->var1,obj->var2);
		PutSpriteLayerTile(x, y, (Word16)tile);
		
		obj->nohit=3+8;
		obj->freeze=0;
		obj->state=sDONE;
		obj->counter=AISPEED*30;
		obj->curframe=0;
		obj->var5++;
		obj->light=0;
		break;


	case sDONE: //wait a few secs, then reappear

		if ((NumPlayers==1) || (MultiPlayerConnection>gameLOCAL))
			break; //never display again!

		obj->counter--;

		if (obj->counter<=0)
		{
			if (obj->var1) //not initialised yet (start skipped because instant deactivation)
			{
				GetNearestPlayer(num,64*64);
				if (Distance>=64*64)
				{
					x=obj->xorg/(32*65536);
					y=obj->yorg/(32*65536);
					PutSpriteLayerTile(x, y, (Word16)obj->var1);
					obj->state=sSLEEP;

					obj->energy=(Word8)obj->var2;
					obj->phase=0;
					obj->nohit=3;
					obj->curframe=anims[obj->curanim].framelist[0]; //collision checking!
				}
			}
			else
			{
				MyDeleteObject(num);//obj->code=NULL;
				SetEventByte(obj->xorg/(32*65536),obj->yorg/(32*65536),obj->load);	//get original block coords
			}
		}
		break;

	case sDEACTIVATE:

		if (obj->var1==0)	//not initialized yet, jumped directly in deactivate!!!
			goto SkipAllThis;


		if (InViewArea(obj->xpos,obj->ypos))	//reenable!!!!
		{
			if (obj->nohit==3+8)
				obj->state=sDONE;
			else
				obj->state=sSLEEP;
		}

		if ((obj->var5==0) || ReInitializingObjects)	//only then, otherwise keep active
		{
			x=obj->xorg/(32*65536);
			y=obj->yorg/(32*65536);
			PutSpriteLayerTile(x, y, (Word16)obj->var1);

SkipAllThis:
			SetEventByte(obj->xorg/(32*65536),obj->yorg/(32*65536),obj->load);	//get original block coords

			MyDeleteObject(num);
		}
		break;

	default:
		FAIL("cDESTRUCTSCENERY(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	} //switch
} //cDESTRUCTSCENERY




void cTRIGGERSCENERY(int num)
{
	Omonster *obj;
	int x,y,tile,calc;

	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		x=obj->xorg/(32*65536);
		y=obj->yorg/(32*65536);
		
		obj->var4=GetEventParm(x,y,0,5);
		tile=GetSpriteLayerTile(x, y);

		obj->var1=tile;	//org animtile
		obj->var2=GetDestructAnimTileFrames(tile)-1;

		if (obj->var2<0)
		{
			DebugXMessage("WARNING: Trigger Scenery at position %i,%i needs at least 2 animation frames. TriggerScenery now deactivated",x,y);
			obj->var2=0;
		}
		else
		if (obj->var2<=0)
			obj->var2=1;
		obj->var3=GetDestructAnimTile(tile,0);

		PutSpriteLayerTile(x, y, (Word16)obj->var3);

		obj->phase=0;

		obj->nohit=3;

		obj->curanim=0;
		obj->curframe=0; //collision checking!
		obj->state=sSLEEP;
		break;

	case sSLEEP:
		if ((TriggerSceneryActive[obj->var4]) && (obj->var5<2))
		{
			obj->counter++;
			if ((obj->counter&3)==0)
			{
				calc=1+(obj->counter/4);
				if (calc>=obj->var2)
				{
					calc=obj->var2;
					obj->var5=2;
				}
				else
					obj->var5=1; //changed!

				tile=GetDestructAnimTile(obj->var1,calc);
				x=obj->xorg/(32*65536);
				y=obj->yorg/(32*65536);

				PutSpriteLayerTile(x, y, (Word16)tile);

			}
		}
		else
		if ((TriggerSceneryActive[obj->var4]==0) && obj->var5)
		{
			obj->var5=0;	//reset it back!!!

			x=obj->xorg/(32*65536);
			y=obj->yorg/(32*65536);
			PutSpriteLayerTile(x, y, (Word16)obj->var3);
		}
		break;

//if not yet changed, we can deactivate this object completely. otherwise, keep it active!
	case sDEACTIVATE:
		obj->state=sSLEEP;

		if ((obj->var5==0) || ReInitializingObjects)	//only then, otherwise keep active
		{
			x=obj->xorg/(32*65536);
			y=obj->yorg/(32*65536);

			if (obj->var1) //not initialised yet (start skipped because instant deactivation)
				PutSpriteLayerTile(x, y, (Word16)obj->var1);

			SetEventByte(x,y,obj->load);	//get original block coords

			MyDeleteObject(num);//obj->code=NULL;
		}
		break;

	default:
		FAIL("cTRIGGERSCENERY(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	} //switch
} //cTRIGGERSCENERY




void cGEMSTOMP(int num)
{
	Omonster *obj;
	Omonster *obj2;
	int t,cobj,numgems;



	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		obj->curframe=anims[obj->curanim].framelist[0]; //collision checking!
		obj->state=sSLEEP;
		obj->ypos-=4*65536;
		break;

	case sSLEEP:
		//do nothin!
		break;

	case sKILL:
		AddExplosion(obj->xpos,obj->ypos,AnimBase[mAMMO]+mAMMO_BOOM1); 

		numgems=5+RandFac(7);
		for (t=0;t<numgems;t++)
		{
			cobj=AddObject(obj->xpos+Rand2Fac(32767)*16,obj->ypos-(16+RandFac(15))*65536,aREDGEMTEMP,0);
			if (cobj>0)
			{
				obj2=(Omonster *)(&gameobj[cobj]);
				obj2->xspeed=Rand2Fac(32767)*32;
				obj2->yspeed=-(int)RandFac(32767)*16;
				obj2->var3=AISPEED/4;
				obj2->objtype=oWAITPICKUP;
			};
		};
		
		MyDeleteObject(num);//obj->code=NULL;
		break;


	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;

	default:
		FAIL("cGEMSTOMP(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //switch

} //cGEMSTOMP



void raven(int num)
{
	Omonster *obj;
	int oldy;
	Word8 oldfreeze;
	int nump;
	int wantx,wanty;
	int wantsx,wantsy,maxspeed;


	obj=(Omonster *)(&gameobj[num]);

	
	switch (obj->state)
	{
	case sSTART:
		obj->state=sIDLE;
		break;

	case sIDLE:
		obj->curanim=AnimBase[mRAVEN]+mRAVEN_FLYCYC;

		obj->phase=(Word8)((gamecounter/6)%anims[obj->curanim].numframes);
		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		obj->ypos+=sintable(gamecounter*4);
		DrawGameobj(num);

		nump=GetNearestPlayer(num,192*192);
		if (nump>=0)
		{	
			obj->state=sFLOAT;
			obj->counter=(768-64)/4;

			if (obj->xpos<player[nump].xpos)
			{
				obj->var2=768;
				obj->var3=4;
			} else
			{
				obj->var2=256;
				obj->var3=-4;
			};

			obj->var1=nump;
		};				
		break;

	case sFLOAT:
		obj->var2+=obj->var3;
		obj->counter--;
		if (obj->counter>0)
		{
			wantx=player[obj->var1].xpos+sintable(obj->var2)*96;
			wanty=player[obj->var1].ypos-64*65536;

			maxspeed=(2+level.difficulty)*65536;

			wantsx=wantx-obj->xpos;
			if (wantsx<-maxspeed) wantsx=-maxspeed;
			else
			if (wantsx>maxspeed) wantsx=maxspeed;

			wantsy=wanty-obj->ypos;
			if (wantsy<-maxspeed) wantsy=-maxspeed;
			else
			if (wantsy>maxspeed) wantsy=maxspeed;

			obj->xpos+=wantsx;
			obj->ypos+=wantsy;

			if (wantsx<-16384)
				obj->direction=-1;
			else
			if (wantsx>16384)
				obj->direction=1;

		} else
		{
			obj->state=sATTACK;
			obj->counter=AISPEED*2;
			obj->curanim=AnimBase[mRAVEN]+mRAVEN_ATTACK;
			obj->var5=(player[obj->var1].xpos-obj->xpos);

		};

		obj->phase=(Word8)((gamecounter/6)%anims[obj->curanim].numframes);
		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		obj->ypos+=sintable(gamecounter*4);
		DrawGameobj(num);
		break;

	case sATTACK:
		wantsx=obj->var5;

		maxspeed=(3+level.difficulty)*65536;

		if (wantsx>maxspeed)
			wantsx=maxspeed;
		else
		if (wantsx<-maxspeed)
			wantsx=-maxspeed;

		obj->xspeed=wantsx/4;
		obj->yspeed=32768;

		obj->xpos+=obj->xspeed;
		obj->ypos+=obj->yspeed;		

		if (obj->xspeed<-16384) obj->direction=-1;
		else
		if (obj->xspeed>16384) obj->direction=1;

		obj->phase=(Word8)((gamecounter/6)%anims[obj->curanim].numframes);
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);

		obj->counter--;
		if ((obj->counter<0) || (obj->ypos+28*65536>player[obj->var1].ypos))
		{
			obj->state=sSTOP;
			obj->counter=0;
			obj->curanim=AnimBase[mRAVEN]+mRAVEN_FLYCYC;
		};
		break;


	case sSTOP:
		wantsx=(obj->xorg-obj->xpos);
		wantsy=(obj->yorg-obj->ypos);

		maxspeed=(4+level.difficulty)*65536;

		if (wantsx>maxspeed)
			wantsx=maxspeed;
		else
		if (wantsx<-maxspeed)
			wantsx=-maxspeed;

		if (wantsy>maxspeed)
			wantsy=maxspeed;
		else
		if (wantsy<-maxspeed)
			wantsy=-maxspeed;

		obj->xspeed=(obj->xspeed+wantsx)/2;
		obj->yspeed=(obj->yspeed+wantsy)/2;

		obj->xpos+=obj->xspeed;
		obj->ypos+=obj->yspeed;		

		if (obj->xspeed<0) obj->direction=-1;
		else
		if (obj->xspeed>0) obj->direction=1;

		obj->phase=(Word8)((gamecounter/6)%anims[obj->curanim].numframes);
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);

		if (((obj->xpos/4096)==(obj->xorg/4096)) && ((obj->ypos/4096)==(obj->yorg/4096)))
		{
			obj->state=sIDLE;
			obj->counter=0;
		};
		break;

	case sFREEZE:
		if (obj->freeze>0) obj->freeze--;
		if (obj->freeze==0) obj->state=obj->oldstate;

		if (obj->freeze<30)
		{
			oldy=obj->ypos;		obj->ypos+=4*Rand2Fac(16383);
			oldfreeze=obj->freeze;	obj->freeze=(Word8)RandFac(1); //let freeze anim flicker

			DrawGameobj(num);
			obj->freeze=oldfreeze;
            obj->ypos=oldy;
		} else
		DrawGameobj(num);

		break;

	case sKILL:
		cKILL(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;

	default:
		FAIL("raven(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //switch
} //raven()




void gemring(int num)
{
	Omonster *obj=(Omonster *)(&gameobj[num]);
	Tplayer *play;
	int px,py;
	int numgems,fase=0;
	int frame;
	int nump;
	int size=0;
	int slot;


	obj->counter++;

	if (obj->state==sSTART)
		obj->state=sSLEEP;
	else if (obj->state==sDEACTIVATE)
	{
		cDEACTIVATE(num);
		return;
	}


	if (obj->state==sSLEEP)
	{
		nump=GetNearestPlayer(num,64*64);
		if (nump>=0)
		{
       		obj->state=sHIT;

			play=&player[nump];
			play->gem[1]+=8;
			if ((play->lastgemcounter/100)<(play->gem[1]/100))
			{	
				slot=FindEmptyMessage(nump,3);
				play->Display[slot].State=3;
				play->Display[slot].Var1=10;
				play->Display[slot].Counter=128;
			}
			else
			{
				slot=FindEmptyMessage(nump,4);
				play->Display[slot].State=4;
				play->Display[slot].Var1=1;
				play->Display[slot].Counter=0;
			}
			play->Display[slot].Xpos=obj->xpos-play->viewstartx;
			play->Display[slot].Ypos=obj->ypos-play->viewstarty;
		}

		obj->var3=64+sintable(obj->counter*8)/4096;

		size=80-(obj->var3-64)/2;
		obj->phase=(obj->counter/4)&7;

		fase=obj->counter*8;
	}
	else if (obj->state==sHIT)
	{
		obj->var2++;
		obj->var3+=obj->var2/8;

		fase=obj->counter*8+obj->var3;
		if (obj->var3>500)
		{
			MyDeleteObject(num);//obj->code=NULL;
			return;
		}

		size=80-(obj->var2/2)-sintable(obj->counter*8)/8192;
		if (size<50) size=50;

		obj->phase=(obj->counter/2)&7;

	}


	obj->curframe=anims[obj->curanim].framelist[obj->phase];


	for (numgems=0;numgems<8;numgems++)
	{
		fase+=128;
		
		px=obj->xpos+(sintable(fase)*obj->var3)/2;
		py=obj->ypos+(costable(fase)*obj->var3)/2;


		frame=anims[obj->curanim].framelist[obj->phase];
		if (obj->state==sSLEEP)
			AddRotSprite(num,px,py,SPRITELAYER,(512+64+fase)/4,size,frame,0);
		else
			AddRotSprite(num,px,py,SPRITELAYER,(512+64+fase+obj->var2*8)/4,size,frame,0);
	}


} //gemring()



void rottree(int num)
{
	Omonster *obj;
	Tplayer *play;
	int px,py;
	int frame;
	int angle;
	int hit=0;
	int sy;
	int calc,p,x0,y0,x1,y1,dx,dy;


	obj=(Omonster *)(&gameobj[num]);


	switch (obj->state)
	{
	case sSTART:
		px=obj->xpos/(32*65536);
		py=obj->ypos/(32*65536);
		calc=GetEventParm(px,py,0,5);

		if (calc==0)
			obj->ypos+=24*65536;
		else
			obj->ypos+=calc*65536;

		obj->xpos+=(16+GetEventParm(px,py,5,-6))*65536;

		obj->phase=0;
		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		obj->var6=frames[obj->curframe].dy;
		
		obj->counter=0;

		obj->var3=0*256; //angle*256
		obj->var4=0; //speed
		obj->state=sSLEEP;
		break;

	case sFREEZE:
		if (obj->freeze>0)
		{
			obj->freeze--;
			if (obj->freeze==0)
				obj->state=obj->oldstate;
		};
	case sSLEEP:
		angle=obj->var3/1024;
		AddRotSprite(num,obj->xpos,obj->ypos,SPRITELAYER,angle,64,obj->curframe,0);
		//staat rechtovereind: kan misschien sneller gedisplayed worden? (bijvoorbeeld de oude RLE sprite)

		if (obj->load!=aROTSMALLTREE)
		{	//this one is not solid!

			p=CheckPlayerSolidBox(num);

			if (p)	//high speed collision, fall over
			{	

			};

/*
//collision! walk against it
			frame=anims[obj->curanim].framelist[obj->phase];

			x0=obj->xpos-frames[frame].dx*2*32768;
			y0=obj->ypos-frames[frame].dy*65536;

			x1=obj->xpos+frames[frame].dx*2*32768;
			y1=obj->ypos+16*65536;

			for (p=0;p<MAXPLAYERS;p++)
			if (player[p].Active)
			{
				play=&player[p];

				hit=0;
				if ((play->ypos>y0) && (play->ypos<y1) &&
					(play->xpos>x0-16*65536) && (play->xpos<x1+16*65536))
				{
					if ((play->xspeed>=0) && (play->xpos<=x0) && (play->xpos+play->xspeed>x0-4*65536))
					{
						play->xpos=x0-4*65536;
						play->xspeed=0;//-play->xspeed/2;//-32768;
					};
					if ((play->xspeed<=0) && (play->xpos>=x1) && (play->xpos+play->xspeed<x1+4*65536))
					{
						play->xpos=x1+4*65536;
						play->xspeed=0;//-play->xspeed/2;//32768;
					};
				}; //if apx near
			}; //for p
*/
		}; //if !small tree
		break;

	case sHIT:
		obj->curframe=0; //delete from collision stuff!
		frame=anims[obj->curanim].framelist[obj->phase];
		obj->counter++;


		if (obj->var3>0)
			obj->var4+=obj->var2+64;
		else
		if (obj->var3<0)
			obj->var4+=obj->var2-64;
		else
			obj->var4+=obj->var2;


		obj->var3+=obj->var4;


//check collision:

		x0=(sintable(obj->var3/256)*obj->var6)/32;
		y0=(costable(obj->var3/256)*obj->var6)/32;

		px=obj->xpos-x0;
		py=obj->ypos-y0;
		for (sy=0;sy<31;sy++)
		{
			hit=CheckPixel(px,py);

			if (hit) break;
			
			//AddSprite(px,py,SPRITELAYER-1,1,spriteNORMAL,0,anims[AnimBase[mAMMO]+mAMMO_BOMB].framelist[0]);

			px-=x0;
			py-=y0;//-65536;
		};


//end of tree only
		px=obj->xpos-sintable(obj->var3/256)*obj->var6;
		py=obj->ypos-costable(obj->var3/256)*obj->var6;
			
		if (CheckForSpring(num,px,py))
		{
			obj->var3-=obj->var4; //angle

			obj->var4=-obj->var4;
			obj->var2=-obj->var2*2;

			hit=0;
		};


		if (hit)
		{
			if (ABS(obj->var4)>256)
			{
				//PlaySample(px,py,sCOMMON_DAMPED1,128,0);
				if (obj->load==aROTSMALLTREE)
					PlaySample(px,py,sSMALTREE_GROUND,0,0);

				AddExplosion(px,py+10*65536,AnimBase[mAMMO]+mAMMO_BOOM1); 
			};

			obj->var3-=obj->var4;
			obj->var4=-obj->var4/4;
			obj->var2=-obj->var2/4;
			if (ABS(obj->var4)<32)
			{
				obj->state=sDONE;
				obj->nohit=1;
			};
		};

		angle=obj->var3/1024;
		AddRotSprite(num,obj->xpos,obj->ypos,SPRITELAYER,angle,64,frame,0);
		ClearSolidBox(num);
		break;

	case sDONE:
		frame=anims[obj->curanim].framelist[obj->phase];
		angle=obj->var3/1024;
		AddRotSprite(num,obj->xpos,obj->ypos,SPRITELAYER,angle,64,frame,0);


//collision! walk over it

		x0=obj->xpos;
		y0=obj->ypos;

		x1=obj->xpos-sintable(angle*4)*frames[frame].dy;
		y1=obj->ypos-costable(angle*4)*frames[frame].dy;

		//AddSprite(x0,y0,SPRITELAYER,1,spriteNORMAL,0,anims[AnimBase[mAMMO]+mAMMO_BOMB].framelist[0]);
		//AddSprite(x1,y1,SPRITELAYER,1,spriteNORMAL,0,anims[AnimBase[mAMMO]+mAMMO_BOMB].framelist[0]);


		if (x1<x0)	//x0=LEFT starting position 
		{
			calc=x0; x0=x1; x1=calc;
			calc=y0; y0=y1; y1=calc;
		};

		dy=y1-y0;
		dx=(x1-x0)/65536;

		for (p=0;p<MAXPLAYERS;p++)
		if (player[p].Active)
		{
			play=&player[p];

			hit=0;
			if ((play->xpos>x0) && (play->xpos<x1))
			{
				calc=y0+(dy*((play->xpos-x0)/65536))/dx;

				if ((play->ypos>calc-28*65536) && 
					(play->ypos<calc+4*65536) && 
					(play->yspeed>-65536))
				{
	
					if (play->downattack<DOWNATTACKLEN)
						play->downattack=DOWNATTACKLEN;

					play->ypos=calc-28*65536+32767; //round off
					play->platform=num;
					play->platformtype=1;
					play->movexspeed=0; //no extra speed here
					play->moveyspeed=0;

					hit=1;
				};
			};

			if (hit==0)
			{			
				if (play->platform==num)
				{
					play->platform=0;
					play->platformtype=0;
					play->movexspeed=0;
					play->moveyspeed=0;
				};
			};
		};
		break;


	case sKILL:
		cKILL(num);
		ClearSolidBox(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		ClearSolidBox(num);
		break;

	default:
		FAIL("rottree(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //switch

} //rottree()



void cAMBIENT(int num)
{
	Omonster *obj;
	int volume;

	obj=(Omonster *)(&gameobj[num]);


	if (obj->state==sDEACTIVATE)
		cDEACTIVATE(num);
	else
	if (obj->state==sKILL)
		MyDeleteObject(num);//obj->code=NULL;
	else
	if (obj->state==sSTART)
	{
		obj->var1=GetEventParm(obj->xpos/(32*65536),obj->ypos/(32*65536),0,8);
		if (obj->var1==0)
			obj->var1=sWIND_WIND2A;
		else
		if (obj->var1==1)
			obj->var1=sCOMMON_BURNIN;
		else
		if (obj->var1==2)
			obj->var1=sSCIENCE_PLOPKAOS;

		obj->var2=GetEventParm(obj->xpos/(32*65536),obj->ypos/(32*65536),8,16)/3;

		obj->var3=GetEventParm(obj->xpos/(32*65536),obj->ypos/(32*65536),16,1);
		obj->var4=GetEventParm(obj->xpos/(32*65536),obj->ypos/(32*65536),17,1);
		obj->state=sSLEEP;
	}
	else
	{
		if (obj->var4)
		{
			volume=obj->var2+
				sintable(gamecounter*4)/8192+
				sintable(gamecounter*6)/8192;

		} else
			volume=obj->var2;
		

		if (obj->var3)
		{
			obj->channel=PlayLoopSample(0,0,obj->var1,volume,0,obj->channel);
			obj->xpos=player[0].xpos;
			obj->ypos=player[0].ypos;
		}
		else
			obj->channel=PlayLoopSample(obj->xpos,obj->ypos,obj->var1,volume,0,obj->channel);

	};

} //cAMBIENT()






void CirclePlatform(int num)
{
	Omonster *obj;
	int n,xp,yp,angle;
	int t,tx,ty,dx;
	int newxpos,newypos;
	int frame;
	int radius,radadd;
	int negacc;
	int angleadd;
	int res;
	int oldangle=0;

	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state) {
	case sSTART:
		xp=obj->xpos/(32*65536);
		yp=obj->ypos/(32*65536);
		obj->var2=GetEventParm(xp,yp,2,-6);		//speed
		obj->var3=GetEventParm(xp,yp,8,4);		//number of elements
		obj->phase=0;
		obj->curframe=anims[obj->curanim].framelist[0];

		if (obj->var2==0) {
			obj->state=sPUSH;
			obj->counter=obj->var5=0;
		} else {
			if (GetEventParm(xp,yp,12,1)) {
				obj->var4=1;
			}
			obj->state=sACTION;
			obj->var1=256*GetEventParm(xp,yp,0,2);  //sync
			obj->var6=obj->var2;
			obj->counter=gamecounter;
			obj->var5=obj->counter*obj->var2+obj->var1;
		}
		break;

	case sPUSH:
		oldangle=obj->var5;
		obj->counter=obj->counter&(256*1024-1);

		if (obj->counter>256*511) {
			negacc=obj->counter/256-1024;
		} else {
			negacc=obj->counter/256;
		}
		if ((obj->var4>32) && (negacc>0)) {
			negacc+=32;
		} else if ((obj->var4<-32) && (negacc<0)) {
			negacc-=32;
		}
		obj->var4-=negacc;

		if (obj->var4>12*256) obj->var4=12*256;
		else
		if (obj->var4<-12*256) obj->var4=-12*256;

		obj->var6=obj->var4/256;

		if (((obj->counter<256) || (obj->counter>1023*256)) && (ABS(obj->var4)<32))
		{
			obj->var4=0;
			obj->counter=0;			
		};

		obj->counter+=obj->var4; //counter=angle*256
		obj->var5=obj->counter/256;
		break;

	case sACTION:
		oldangle=obj->var5;
		obj->counter++;
		obj->var5=obj->counter*obj->var2+obj->var1;
		obj->var6=obj->var2;

		if (obj->var4==1)
			oldangle=1024+sintable(obj->var5)/256;
		//drawing is done below!
		break;


	case sFREEZE:
		oldangle=obj->var5;

		obj->freeze--;
		if (obj->freeze<=0)
		{
			if (obj->var2==0)
			{
				obj->state=sPUSH;
				obj->var5=obj->counter/256;
			}
			else
			{
				if (GetEventParm(obj->xpos/(32*65536),obj->ypos/(32*65536),12,1))
					obj->state=sFLY;
				else
					obj->state=sACTION;
				//obj->var5=(obj->counter+obj->var1*256)*obj->var2;
			};
		};
		//still drawn below
		break;

	case sKILL:
		ClearPlayerPlatform(num);	//clear remaining platform

		frame=anims[obj->killanim].framelist[0];
		radadd=frames[frame].dx-1;
		radius=0;

		
		angleadd=-obj->var6;
		angle=obj->var5-angleadd*obj->var3;

		for (n=0;n<obj->var3;n++)
		{
			xp=obj->xorg+radius*sintable(angle);
			yp=obj->yorg+radius*costable(angle);

			AddSprite(xp,yp,SPRITELAYER,1,spriteNORMAL,0,frame);

			res=AddObject(xp,yp,aSHARD,0);
			if (res>0)
			{
				gameobj[res].curanim=obj->killanim;
				gameobj[res].state=sACTION;
				if ((angle&1023)>512)
					((Omonster *)(&gameobj[res]))->xspeed=8192;
				else
					((Omonster *)(&gameobj[res]))->xspeed=-8192;
			};

			angle+=angleadd;
			radius+=radadd;
		};

		newxpos=obj->xorg+radius*sintable(angle);
		newypos=obj->yorg+radius*costable(angle);

		obj->xspeed=0;//(newxpos-obj->xpos)/8;
		obj->yspeed=0;//newypos-obj->ypos;

		obj->xpos=newxpos;
		obj->ypos=newypos;

		DrawGameobj(num);
		obj->objtype=oPARTICLE;
		obj->nohit=3; 
		obj->energy=10;
		obj->state=sACTION;
		obj->ProcPtr=&cRANDPART;

		return;
		break;


	case sDEACTIVATE:
		ClearPlayerPlatform(num);	//clear remaining platform
		cDEACTIVATE(num);
		break;

	default:
		FAIL("CirclePlatform(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //obj->state


//do common stuff




//DRAW::

	frame=anims[obj->killanim].framelist[0];
	radadd=frames[frame].dx-1;
	radius=0;

	
	angleadd=-obj->var6;
	angle=oldangle-angleadd*obj->var3;


	for (n=0;n<obj->var3;n++)
	{
		xp=obj->xorg+radius*sintable(angle);
		yp=obj->yorg+radius*costable(angle);
	
		if (obj->freeze)
			AddSprite(xp,yp,SPRITELAYER,1,spriteFREEZE,0,frame);
		else
			AddSprite(xp,yp,SPRITELAYER,1,spriteNORMAL,0,frame);

		angle+=angleadd;
		radius+=radadd;
	};

	newxpos=obj->xorg+radius*sintable(angle);
	newypos=obj->yorg+radius*costable(angle);


	obj->xspeed=newxpos-obj->xpos;
	obj->yspeed=newypos-obj->ypos;

	obj->xpos=newxpos;
	obj->ypos=newypos;

	DrawGameobj(num);


	if (obj->load!=aSPIKEBOLL) //only when you can stand on it!
	{	//collision

		dx=frames[obj->curframe].dx;

		for (t=0;t<MAXPLAYERS;t++)
		if (player[t].Active)
		{
			tx=(player[t].xpos-obj->xpos)/65536;
			ty=(player[t].ypos-obj->ypos)/65536;


			if (player[t].platform==num)
			{
				if ((tx>-dx/2) && (tx<dx/2) &&
					(ty>-28) && (ty<0))
				{
					player[t].downattack=DOWNATTACKLEN+AISPEED; //disable
					//need +aispeed because of diving!

					player[t].platform_rely=-24*65536;

					player[t].movexspeed=0;//obj->xspeed;
					player[t].moveyspeed=0;//obj->yspeed;
				} else
				{
					player[t].platform=0;
					player[t].platformtype=0;
					player[t].movexspeed=0;
					player[t].moveyspeed=0;
				};

			} else
			{
				if ((tx>-dx/2) && (tx<dx/2) &&
					(ty>-28) && (ty<-20) &&
					(player[t].yspeed>obj->yspeed-65536))
				{
					player[t].downattack=DOWNATTACKLEN+AISPEED; //disable

					player[t].platform_rely=-24*65536;
					player[t].platform_relx=player[t].xpos-obj->xpos;

					player[t].platform=num;
					player[t].platformtype=2;

					player[t].movexspeed=0;//obj->xspeed;
					player[t].moveyspeed=0;//obj->yspeed;
				}
			}
		}
	}

} //CirclePlatform()



void cGENERATOR(int num)
{
	Omonster *obj;
	int trigtype;


	obj=(Omonster *)(&gameobj[num]);


	if (obj->state==sSTART)
	{
		obj->state=sSTILL;
		obj->curframe=0;

		obj->var1=-1;
		obj->var2=0;
		obj->var3=AISPEED*GetEventParm(obj->xpos/(32*65536),obj->ypos/(32*65536),8,5);		
		if (obj->var3==0)
			obj->var3=4*AISPEED;
		obj->var4=GetEventParm(obj->xpos/(32*65536),obj->ypos/(32*65536),0,8);

		obj->var6=0;	//dont! trigger first time this gets active

	} else
	if (obj->state==sDEACTIVATE)
	{
		cDEACTIVATE(num);
		return;
	} else
	if (obj->state==sKILL)
	{
		MyDeleteObject(num);//obj->code=NULL;
		return;
	};





	if ((MultiPlayerConnection>gameLOCAL) && !LocalComputerIsServer)
	{
		if (obj->var6&1)
		{
			obj->var2=0;		//trigger object
		}
		else
		if (obj->var6&2)
		{
			obj->var2=AISPEED/2;	//trigger explosion
		}
		else
		{
			obj->var2=AISPEED;	//nothing........
		}

//		obj->var1=0;	//client doesnt need to kno
		obj->var6=0;
	}
	else
	{
		if ((obj->var1>0) && (gameobj[obj->var1].ProcPtr) && (gameobj[obj->var1].creator==LEVELSLOT+num))
			obj->var2=obj->var3; //reset counter;	//dont do nothing
		else
		{
			if (obj->var2>0)
				obj->var2--;
		}
	}


	trigtype=0;
	if (obj->var2==AISPEED/2)
	{
		AddExplosion(obj->xpos,obj->ypos,AnimBase[mPICKUPS]+mPICKUPS_SPRKLE);
		trigtype=2;
	}
	else
	if (obj->var2==0)	//countdown till zero
	{
		obj->var1=AddObject(obj->xpos,obj->ypos,obj->var4,LEVELSLOT+num);	
		//needs to be levelslot for good collision in network situations,
		//the UID is then rightly calculated for collisions over the network (so that each
		//if the generators-object number is used this will not work, since the object number
		//will probably differ between the client and the server

		trigtype=1;
		obj->var2=obj->var3; //reset counter
	}

	if ((MultiPlayerConnection>gameLOCAL) && LocalComputerIsServer && trigtype) {
		TactPACKETS *action;
		Word8 data[32];

		action=(TactPACKETS *)&data[0];
		action->GENERIC.PacketID=actTRIGGERGENERATOR;
		action->TRIGGERGENERATOR.UIDLow=UID[num];
		action->TRIGGERGENERATOR.UIDHigh=UID[num]>>8;
		action->TRIGGERGENERATOR.type=trigtype;
		Server_SendActions(action,0);	//send to all!
	}

} //cGENERATOR()








void cAIRBOARDGENERATOR(int num)
{
	Omonster *obj;


	obj=(Omonster *)(&gameobj[num]);


	if (obj->state==sSTART)
	{
		obj->state=sSTILL;
		obj->curframe=0;

		obj->var1=-1;
		obj->var2=0;
		obj->var3=AISPEED*30;//GetEventParm(obj->xpos/(32*65536),obj->ypos/(32*65536),8,5);		
		if (obj->var3==0)
			obj->var3=4*AISPEED;
		obj->var4=aAIRBOARD;//GetEventParm(obj->xpos/(32*65536),obj->ypos/(32*65536),0,8);


	} else
	if (obj->state==sDEACTIVATE)
	{
		cDEACTIVATE(num);
		return;
	} else
	if (obj->state==sKILL)
	{
		MyDeleteObject(num);//obj->code=NULL;
		return;
	};

	if (obj->var2>0)
		obj->var2--;

	if (obj->var2==AISPEED/2)
	{
		if ((obj->var1>0) && (gameobj[obj->var1].ProcPtr) && (gameobj[obj->var1].creator==num))
			;
		else
			AddExplosion(obj->xpos,obj->ypos,AnimBase[mPICKUPS]+mPICKUPS_SPRKLE);
	}

	if (obj->var2==0)	//countdown till zero
	{
		
		if ((obj->var1>0) && (gameobj[obj->var1].ProcPtr) && (gameobj[obj->var1].creator==num))
			;
		else
			obj->var1=AddObject(obj->xpos,obj->ypos,obj->var4,num);

		obj->var2=obj->var3; //reset counter
	}
	

} //cAIRBOARDGENERATOR()












void tubeturtle(int num)
{
	Omonster *obj;
	int go,waterlevel;

	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		obj->state=sFLOAT;
		
		obj->counter=RandFac(255); //phase etc
		break;

	case sFLOAT:

		waterlevel=level.waterlevel-8*65536;
//DO THE Y PART
		
		if (obj->ypos<waterlevel-4*65536) 
		{
			obj->yspeed+=level.gravity;
			if (obj->yspeed>4*65536)
				obj->yspeed=4*65536;

			go=CheckPixel(obj->xpos,obj->ypos+14*65536);
			if (go==0)
				obj->ypos+=obj->yspeed;
			else
				obj->yspeed=0;
		}
		else
		{
			if (obj->ypos>waterlevel+4*65536)
			{
				obj->yspeed-=level.gravity/8;

				go=CheckPixel(obj->xpos,obj->ypos+obj->yspeed);

				if (go==0)
					obj->ypos+=obj->yspeed;
				else
					obj->yspeed=0; //hang under water
			}
			else
			{
				obj->ypos=waterlevel;
				obj->yspeed=0;
			}
		}

		if (ABS(obj->ypos-waterlevel)<4*65536) //do horizontal movement on top of waterlevel)
		{

			obj->xspeed=sintable(obj->counter*4);
			obj->xpos+=obj->xspeed;

			if (obj->xspeed<0)
				obj->direction=-1;
			else
				obj->direction=1;
/*


			if (obj->direction>0)
			{
				go=CheckPixel(obj->xpos+8*65536,obj->ypos);

				if ((go!=0) || (tileattr==areaSTOPENEMY)) //dont move that way
				{
					obj->xspeed=0;
					obj->direction=-1;

				} else
				{
					obj->xspeed+=2048;
					if (obj->xspeed>2*65536)
						obj->xspeed=2*65536;
				}

				obj->xpos+=obj->xspeed;
			}
			else
			{
				go=CheckPixel(obj->xpos-8*65536,obj->ypos);

				if ((go!=0) || (tileattr==areaSTOPENEMY)) //dont move that way
				{
					obj->xspeed=0;
					obj->direction=1;

				} else
				{
					obj->xspeed-=2048;
					if (obj->xspeed<-2*65536)
						obj->xspeed=-2*65536;

					obj->xpos+=obj->xspeed;
				}
			}
*/
		}

		obj->counter++;

		obj->phase=(obj->counter/8)%anims[obj->curanim].numframes;

		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;

	case sFREEZE:
        if (obj->freeze>0) obj->freeze--;
        if (obj->freeze==0) obj->state=obj->oldstate;
		DrawGameobj(num);
		break;

	case sKILL:
		cKILL(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;

	default:
		FAIL("tubeturtle(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}	//obj->state

} //tubeturtle()



static int EVAKISSES[4]={sEVA_KISS1,sEVA_KISS2,sEVA_KISS3,sEVA_KISS4};
void cEVA(int num)
{
	Omonster *obj;
	int dist;
	int nearest,nearp;
	int dx,dy;
	int tx,ty;
	int t;


	obj=(Omonster *)(&gameobj[num]);

	if (obj->state==sSTART)
	{
		PutWalkerOnGround(num);
		obj->state=sSTILL;
	};
	

	if (obj->state==sSTILL)
	{
		tx=obj->xpos-32*65536;
		ty=obj->ypos;

		nearest=16*16;
		nearp=-1;

		for (t=0;t<MAXPLAYERS;t++)
		if ((player[t].Active) && (player[t].character==mFROG))
		{
			dx=(player[t].xpos-tx)/65536;
       		dy=(player[t].ypos-ty)/65536;

       		dist=dx*dx+dy*dy;
			if (dist<nearest)
			{
				nearest=dist;
				nearp=t;
			}
		}

		if (nearp>=0)
		{
			obj->var1=nearp;
			obj->state=sACTION;
			obj->counter=0;
			obj->curanim=AnimBase[mEVA]+mEVA_SMOOCH;
			obj->phase=0;
		}
		else		
		{
			obj->counter++;

			if (obj->curanim==(int)AnimBase[mEVA]+mEVA_BLINK)
			{
				if (obj->counter>8)
				{
					obj->counter=0;
					obj->phase++;
					if (obj->phase>=(int)anims[obj->curanim].numframes)
					{
						obj->phase=0;
						obj->curanim=AnimBase[mEVA]+mEVA_IDLE;
					}
				}
			}
			else
			{
				if (RandFac(63)==0)
				{
					obj->curanim=AnimBase[mEVA]+mEVA_BLINK;
					obj->phase=0;
					obj->counter=0;
				}
			}
		}
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
	}
	else
	if (obj->state==sACTION)
	{

//hold player tight!
		player[obj->var1].xpos=obj->xpos-32*65536;
		player[obj->var1].xspeed=0;
		player[obj->var1].xacc=0;
		player[obj->var1].ypos=obj->ypos+10*65536;
		player[obj->var1].yspeed=0;
		player[obj->var1].yacc=0;
		player[obj->var1].bemoved=2;

		obj->counter++;
		if (obj->curanim==(int)AnimBase[mEVA]+mEVA_SMOOCH)
		{
			if (obj->counter>8)
			{
				obj->counter=0;
				obj->phase++;
				if (obj->phase==11)
				{
					PlaySample(obj->xpos,obj->ypos,EVAKISSES[RandFac(3)],0,0);
				}

				if (obj->phase>=(int)anims[obj->curanim].numframes)
				{
					obj->phase=0;
					obj->curanim=AnimBase[mEVA]+mEVA_STANDUP;

					player[obj->var1].character=player[obj->var1].orgcharacter;
					player[obj->var1].frogmorph=20*7;
				}
			}
		}
		else
		{
			if (obj->counter>8)
			{
				obj->counter=0;
				obj->phase++;
				if (obj->phase>=(int)anims[obj->curanim].numframes)
				{
					obj->phase=0;
					obj->state=sSTILL;
					obj->curanim=AnimBase[mEVA]+mEVA_IDLE;
				}
			}
		}

		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
	} else
	if (obj->state==sKILL) 
		cKILL(num);
	else
	if (obj->state==sDEACTIVATE)
		cDEACTIVATE(num);

} //cEVA()



void cBUBBLER(int num)
{
	Omonster *obj;
	int res;

	obj=(Omonster *)(&gameobj[num]);

	if (obj->state==sSTART)
	{
		obj->ypos+=16*65536; //on grond
		obj->var1=GetEventParm(obj->xpos/(32*65536),obj->ypos/(32*65536),0,4);

		obj->state=sSTILL;
	}
	else
	if (obj->state==sSTILL)
	{
		if ((obj->ypos+10*65536>level.waterlevel) && ((int)RandFac(15)<=obj->var1))
		{
			res=AddObject(obj->xpos,obj->ypos,aBUBBLE,0);
			if ((res>0) && (RandFac(1)))
				gameobj[res].phase=1+(Word8)RandFac(1);
		};
	} else
	if (obj->state==sKILL) 
		cKILL(num);
	else
	if (obj->state==sDEACTIVATE)
		cDEACTIVATE(num);

} //cBUBBLER()




void Swinger3D(int num)
{
	Omonster *obj;
	int xp,yp;
	int n,angle,angleadd;
	int depth;
	int frame;
	int radius,radadd;


	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		
		xp=obj->xpos/(32*65536);
		yp=obj->ypos/(32*65536);
		obj->var6=GetEventParm(xp,yp,2,-6); //speed
		obj->var3=GetEventParm(xp,yp,8,4); //number of elements

		obj->phase=0;
		obj->curframe=anims[obj->curanim].framelist[0];

		if (GetEventParm(xp,yp,12,1))
			obj->var4=1;

		if (GetEventParm(xp,yp,13,1))
			obj->var4|=2;


		obj->state=sACTION;
		obj->counter=gamecounter;  //sync
		obj->var1=256*GetEventParm(xp,yp,0,2);
		obj->var5=obj->counter*obj->var6+obj->var1;

		break;

	case sKILL:
		cKILL(num);
		return;
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		return;
		break;

	case sFREEZE:
		if (GetEventParm(obj->xpos/(32*65536),obj->ypos/(32*65536),12,1))
			obj->state=sFLY;
		else
			obj->state=sACTION;
		obj->freeze=0;
		break;

	case sACTION:
		obj->counter++;

		obj->var5+=obj->var6;
		break;

	default:
		FAIL("swinger3d(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;

	}; //switch

	frame=anims[obj->killanim].framelist[0];
	radadd=frames[frame].dx-1;
	radius=0;

	angleadd=-obj->var6;

	if ((obj->var4&1)==1)
		angle=1024+sintable(obj->var5)/256;
	else
		angle=obj->var5&1023;//(obj->var5-angleadd*obj->var3)&1023;


	if ((obj->var5&1023)<512)
	{
	//	radius+=32;

		radius=obj->var3*radadd;
		depth=(128*65536+radius*sintable(angle))/65536;

		if (!depth) depth=1; //hihi
		obj->ypos=obj->yorg+(radius*128*costable(angle))/depth;

		frame=anims[obj->curanim].framelist[0];

		if (obj->var4&2)
			AddRotSprite(num,obj->xpos,obj->ypos,SPRITELAYER,0,depth/2,frame,(256-depth)/4);
		else
			AddRotSprite(num,obj->xpos,obj->ypos,SPRITELAYER,0,depth/2,frame,0);


		frame=anims[obj->killanim].framelist[0];

		//radius-=16;
		for (n=0;n<=obj->var3;n++)
		{
			depth=(128*65536+radius*sintable(angle))/65536;

			if (!depth) depth=1; //hihi
			yp=obj->yorg+(radius*128*costable(angle))/depth;

			if (obj->var4&2)
				AddRotSprite(num,obj->xorg,yp,SPRITELAYER,0,depth/2,frame,(256-depth)/4);	
			else
				AddRotSprite(num,obj->xorg,yp,SPRITELAYER,0,depth/2,frame,0);	

			radius-=radadd;
		};

	} else
	{

		for (n=0;n<obj->var3;n++)
		{
			depth=(128*65536+radius*sintable(angle))/65536;

			if (!depth) depth=1; //hihi
			yp=obj->yorg+(radius*128*costable(angle))/depth;

			AddRotSprite(num,obj->xorg,yp,SPRITELAYER-1,0,depth/2,frame,0);	

			radius+=radadd;
		};


		radius=obj->var3*radadd;
		depth=(128*65536+radius*sintable(angle))/65536;

		if (!depth) depth=1; //hihi
		obj->ypos=obj->yorg+(radius*128*costable(angle))/depth;

		frame=anims[obj->curanim].framelist[0];

		AddRotSprite(num,obj->xpos,obj->ypos,SPRITELAYER-1,0,depth/2,frame,0);
			
	};


	radius=obj->var3*radadd;
	depth=(128*65536+radius*sintable(angle))/65536;

	if ((depth>108) && (depth<148))
		obj->curframe=anims[obj->curanim].framelist[0];
	else
		obj->curframe=0;

/*	
	if (obj->curframe)
	{
		int curanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
		int curframe=anims[curanim].framelist[0];
		AddSprite(obj->xorg,obj->yorg,SPRITELAYER,0,0,0,curframe);
	}
*/
	
} //Swinger3D()


/*
static float string1x[32];
static float string1y[32];
static float velox[32];
static float veloy[32];
static float string2x[32];
static float string2y[32];


float vlen(float x,float y)
{
	return (float)sqrt(x*x+y*y);
};


void SpringCord(int num)
{
	Omonster *obj;
	int t;
	Word i;
	float xv1,yv1,mag1,ex1,xv2,yv2,mag2,ex2,xv,yv;
	int x,y;


	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		
		for (t=0;t<32;t++)
		{
			string1x[t]=t*(float)1.0; 
			string1y[t]=(float)0.0;
			velox[t]=(float)0.0;
			veloy[t]=(float)0.0;
		};
		
		obj->state=sACTION;

		break;

	case sACTION:
		//velox[15]=(float)(player[0].xspeed)/655360.0;
		//veloy[15]=(float)(player[0].yspeed)/655360.0;

		string1x[15]=(-obj->xpos+player[0].xpos)/((float)65536.0*(float)8.0);
		string1y[15]=(-obj->ypos+player[0].ypos)/((float)65536.0*(float)8.0);
	
		for (i=1;i<31;i++)
		{
			xv1=string1x[i-1]-string1x[i];
			yv1=string1y[i-1]-string1y[i];
			mag1=vlen(xv1,yv1);
			ex1=mag1-(float)1.0;

			xv2=string1x[i+1]-string1x[i];
			yv2=string1y[i+1]-string1y[i];
			mag2=vlen(xv2,yv2);
			ex2=mag2-(float)1.0;

			xv=((xv1/mag1)*ex1)+((xv2/mag2)*ex2);
			yv=((yv1/mag1)*ex1)+((yv2/mag2)*ex2)+(float)0.21;

			velox[i]=velox[i]*(float)0.97+xv*(float)0.01;
			veloy[i]=veloy[i]*(float)0.97+yv*(float)0.01;

			string2x[i]=string1x[i]+velox[i];
			string2y[i]=string1y[i]+veloy[i];
		};

		FastMemCpy(string1x,string2x,sizeof(string1x));
		FastMemCpy(string1y,string2y,sizeof(string1y));


		for (i=0;i<16;i++)
		{
			x=obj->xpos+(int)(string1x[i]*(float)4.0*(float)65536.0);
			y=obj->ypos+(int)(string1y[i]*(float)4.0*(float)65536.0);


			AddSprite(x,y,SPRITELAYER,1,spriteNORMAL,0,anims[AnimBase[mAMMO]+mAMMO_BOMB].framelist[0]);
		};


		break;

	case sKILL:
		cKILL(num);
		return;
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		return;
		break;

	default:
		FAIL("springcord(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //switch


} //SpringCord

*/



void fish(int num)
{
	Omonster *obj;
	int p;


	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
	case sSTILL:
		if (obj->ypos>level.waterlevel)
			obj->state=sFLOAT;

		obj->counter++;
		if ((obj->counter&15)==0)
		{
			obj->phase++;
			if (obj->phase>=(int)anims[obj->curanim].numframes)
				obj->phase=0;
		};
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;

	case sFLOAT:

		obj->xpos=obj->xorg+sintable(obj->counter)*64;
		obj->ypos=obj->yorg+costable(2*obj->counter)*64;

		if (obj->ypos<level.waterlevel)
			obj->ypos=level.waterlevel;

		if (((obj->counter+256)&1023)<512)
			obj->direction=1;
		else
			obj->direction=-1;


		obj->counter++;

		obj->curanim=AnimBase[mFISH]+mFISH_SWIM;
		if ((obj->counter&7)==0)
		{
			obj->phase++;
			if (obj->phase>=(int)anims[obj->curanim].numframes)
				obj->phase=0;
		};

		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);

		if (obj->counter>AISPEED*4)
		{
			p=GetNearestPlayer(num,160*160);
			if (p>=0)
       		{
				obj->var1=p; //target!

				obj->var2=(player[p].xpos-obj->xpos)/64;
				obj->var3=(player[p].ypos-obj->ypos)/64;

				if (obj->var2<0)
					obj->direction=-1;
				else
					obj->direction=1;


				obj->phase=0;
				obj->state=sATTACK;
				obj->counter=0;
				obj->curanim=AnimBase[mFISH]+mFISH_ATTACK;
			};
		};
		break;


	case sATTACK:

		obj->xpos+=obj->var2;
		obj->ypos+=obj->var3;


		if (obj->ypos<level.waterlevel)
			obj->ypos=level.waterlevel;
		
		obj->counter++;
		if ((obj->counter&7)==0)
		{
			obj->phase++;
			if (obj->phase>=(int)anims[obj->curanim].numframes)
				obj->phase=0;
		};
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);


		if (obj->counter>64+AISPEED/2)
		{
			obj->state=sEXTRA;
			obj->counter=0;
			obj->phase=0;

			obj->curanim=AnimBase[mFISH]+mFISH_SWIM;
			obj->var2=(obj->xorg-obj->xpos)/64;
			obj->var3=(obj->yorg+64*65536-obj->ypos)/64;
			if (obj->var2<0)
				obj->direction=-1;
			else
				obj->direction=1;
		};
		break;

	case sEXTRA: //return towards normal position!
		obj->xpos+=obj->var2;
		obj->ypos+=obj->var3;
		
		obj->counter++;
		if ((obj->counter&7)==0)
		{
			obj->phase++;
			if (obj->phase>=(int)anims[obj->curanim].numframes)
				obj->phase=0;
		};
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);

		if (obj->counter>63)
		{
			obj->state=sFLOAT;
			obj->counter=0;
			obj->phase=0;
		};
		break;

	case sFREEZE:
        if (obj->freeze>0) obj->freeze--;
        if (obj->freeze==0) obj->state=obj->oldstate;
		DrawGameobj(num);
		break;

	case sKILL:
		cKILL(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;

	default:
		FAIL("fish(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //obj->state

} //fish()





void rapier(int num)
{
	Omonster *obj;
	int p;
	


	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		obj->state=sFLOAT;
		break;

	case sFLOAT:

		obj->xpos=obj->xorg+sintable(obj->counter)*16;
		obj->ypos=obj->yorg+costable(2*obj->counter)*16;

		obj->counter++;

		obj->curanim=AnimBase[mRAPIER]+mRAPIER_IDLE;
		obj->phase=(obj->counter/8)%anims[obj->curanim].numframes;
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);

		if (obj->counter>AISPEED*4)
		{
			p=GetNearestPlayer(num,256*128);
			if ((p>=0) && (p<MAXPLAYERS))
       		{
				if (player[p].xpos>obj->xpos)
					obj->direction=1;
				else
					obj->direction=-1;

				if (((player[p].ypos+65536*4-1)/(65536*8))==((obj->ypos+65536*4-1)/(65536*8)))
				{
					obj->var1=p;
					obj->phase=0;
					obj->state=sFLY;
					obj->counter=0;
					obj->curanim=AnimBase[mRAPIER]+mRAPIER_REVUP;
					obj->var6=0; //substate
				}
			}
		}
		break;

	case sFLY:
		if (obj->var6==0)
		{	//REVVING UP!
			obj->counter++;

			if ((obj->counter&3)==0)
			{
				obj->phase++;
				if (obj->phase>=(int)anims[obj->curanim].numframes)
				{	//WHEN ANIMATION IS DONE START FLYING!
					obj->phase=0;

					if (obj->direction<0)
						obj->xacc=-4096;
					else
						obj->xacc=4096;
					obj->xspeed=0;

					obj->yspeed=0;	//bit of YSPEED looks better

					obj->phase=0;
					obj->var6=1;
					obj->counter=0;
					obj->curanim=AnimBase[mRAPIER]+mRAPIER_ATTACK;
				}
			}
		} else
		if (obj->var6==1)
		{	//ATTACKING WITH SWORD PROTRUDED
			obj->xspeed+=obj->xacc;
			obj->xpos+=obj->xspeed;

			obj->yspeed+=obj->yacc;
			obj->yacc=(player[obj->var1].ypos-obj->ypos)/256;

			obj->ypos+=obj->yspeed;

			obj->counter++;

			if ((obj->counter&7)==0)
			{
				obj->phase++;
				if (obj->phase>=(int)anims[obj->curanim].numframes)
					obj->phase=0;
			};

			if (obj->counter>AISPEED)
			{
				obj->var6=2;
				obj->counter=0;
				obj->phase=0;
				obj->curanim=AnimBase[mRAPIER]+mRAPIER_SKID;
			}
		} else
		if (obj->var6==2)
		{	//SKIDDING
			obj->xspeed=(obj->xspeed*7)/8;;
			obj->xpos+=obj->xspeed;

			obj->yspeed=(obj->yspeed*7)/8;
			obj->ypos+=obj->yspeed;
			obj->counter++;

			if ((obj->counter&3)==0)
			{
				obj->phase++;
				if (obj->phase>=(int)anims[obj->curanim].numframes)
				{	//STOP SKIDDING WHEN ANIMATION IS DONE!
					obj->var6=3;
					obj->counter=0;
					obj->phase=0;

					obj->curanim=AnimBase[mRAPIER]+mRAPIER_IDLE;

					obj->var2=(obj->xorg-obj->xpos)/64;
					obj->var3=(obj->yorg+16*65536-obj->ypos)/64;
					if (obj->var2<0)
						obj->direction=-1;
					else
						obj->direction=1;
				}
			}
		} else
		if (obj->var6==3)
		{	//FLYING BACK TO ORIGINAL POSITION (figure 8 FLOAT pattern)
			obj->xpos+=obj->var2;
			obj->ypos+=obj->var3;
			
			obj->counter++;
			if ((obj->counter&7)==0)
			{
				obj->phase++;
				if (obj->phase>=(int)anims[obj->curanim].numframes)
					obj->phase=0;
			};

			if (obj->counter>63)
			{
				obj->state=sFLOAT;	//HERE WE GO AGAIN
				obj->counter=0;
				obj->phase=0;
			}
		}
		else
			obj->var6=0;

		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;


	case sFREEZE:
        if (obj->freeze>0) obj->freeze--;
        if (obj->freeze<=0) obj->state=obj->oldstate;
		DrawGameobj(num);
		break;

	case sKILL:
		cKILL(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;

	default:
		FAIL("rapier(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //obj->state

} //rapier()


void bees(int num)
{




} //bees()




void cLASER(int num)
{
	Omonster *obj;

	obj=(Omonster *)(&gameobj[num]);

	obj->state=sFLY; //do collision

	if (++obj->counter>3)
		MyDeleteObject(num);//obj->code=NULL;

	//obj->lighttype=6;
	//obj->light=127-16*(Word8)obj->counter; //flicker

} //cLASER()







void cCORPSE(int num)
{
	Tgameobj *obj;	

	obj=&gameobj[num];

	obj->counter++;
	if ((obj->counter&7)==0)
	{
		obj->phase++;
		if (obj->phase>=(int)anims[obj->curanim].numframes)
			obj->phase=0;
	};

	obj->curframe=anims[obj->curanim].framelist[obj->phase];
	DrawGameobj(num);

} //cCORPSE()


void cUFO(int num)
{
	Omonster *obj;
	int calc;

	obj=(Omonster *)(&gameobj[num]);

	if (obj->state==sSTART)
	{
		obj->state=sFLY;
		obj->curanim=AnimBase[mSPAZ2]+mSPAZ2_UFO;
		obj->light=1;
		obj->lighttype=2;

		obj->xpos+=ScreenWidth*32768;
		obj->ypos-=ScreenHeight*24000;
		
		obj->yspeed=ScreenHeight*24000/16;
		obj->xspeed=(int)ScreenWidth*-32768/(10*8);
		obj->var1=1;

	} else
	if ((obj->state==sDEACTIVATE) || (obj->state==sKILL))
	{
		MyDeleteObject(num);//obj->code=NULL;
	};

	obj->counter++;

	if (obj->counter<10*8)
	{
		obj->xpos+=obj->xspeed;

		if ((obj->counter&7)==0)
			PlaySample(obj->xpos,obj->ypos,sCOMMON_PREEXPL1,100,0);

		if (obj->counter==8*8)
			PlaySample(obj->xorg,obj->yorg,sAMMO_GUN3PLOP,0,22050);
		if (obj->counter>=8*8)
			AddSprite(obj->xorg,obj->yorg-(obj->counter-8*8)*obj->yspeed,SPRITELAYER,1,spriteNORMAL,0,anims[AnimBase[mAMMO]+mAMMO_GUN1U].framelist[0]);
	}
	else
	{
		if (obj->counter==10*8)
		{
			obj->xspeed/=4;
			obj->yspeed=0;
			AddExplosion(obj->xpos,obj->ypos,AnimBase[mAMMO]+mAMMO_BOOM4); 

			PlaySample(obj->xpos,obj->ypos,sCOMMON_EXPL_TNT,128,0);
		}
		else
		if ((obj->counter&3)==0)
		{
			AddExplosion(obj->xpos,obj->ypos,AnimBase[mAMMO]+mAMMO_POOF2); 

			if (RandFac(3)==1)
				PlaySample(obj->xpos,obj->ypos,sCOMMON_PREEXPL1,40+RandFac(63),0);
		}


		obj->ypos+=obj->yspeed;
		obj->yspeed+=level.gravity;
		
		obj->xpos+=obj->xspeed;
		obj->xspeed-=1024;


		if (obj->var1 && CheckPixel(obj->xpos,obj->ypos))
		{
			obj->var1=0;
			obj->yspeed=0;
			PlaySample(obj->xpos,obj->ypos,sCOMMON_METALHIT,128,0);
		}
		
		if (obj->ypos>obj->yorg+240*65536)
		{
			MyDeleteObject(num);
			return;
		}
	}

	if ((obj->counter&3)==3)
	{
		obj->phase++;
		if (obj->phase>2)
			obj->phase=0;
	};

	obj->curframe=anims[obj->curanim].framelist[obj->phase];
	calc=obj->counter*2;
	if (calc<32)
		AddScaledSprite(obj->xpos,obj->ypos,SPRITELAYER,calc,obj->curframe);
	else
		AddSprite(obj->xpos,obj->ypos,SPRITELAYER,1,spriteNORMAL,0,obj->curframe);

}	//cUFO()



//money bunny!!
void cWARP(int num)
{
	Omonster *obj;
	Tplayer *play;
	int nump;
	int dx,dy;
	int px,py;
	int check,calc;
	int numevent;
	int warpid;
	Word8 *DumpBuf;


	obj=(Omonster *)(&gameobj[num]);


	if (obj->state==sDEACTIVATE)
	{
		cDEACTIVATE(num);
	}
	else
	if (obj->state==sSTART)
	{
		if (GetEventParm(obj->xorg/(32*65536),obj->yorg/(32*65536),17,1)==0)
		{
			MyDeleteObject(num);//obj->code=NULL; //dont show anim!
			return;
		}
		obj->var1=GetEventParm(obj->xorg/(32*65536),obj->yorg/(32*65536),8,8);

		if (obj->var1==10)
			obj->curanim=AnimBase[mWARP10];
		else
		if (obj->var1==20)
			obj->curanim=AnimBase[mWARP20];
		else
		if (obj->var1==50)
			obj->curanim=AnimBase[mWARP50];
		else
		if (obj->var1==100)
			obj->curanim=AnimBase[mWARP100];
	
		obj->phase=0;
		obj->curframe=anims[obj->curanim].framelist[0];
	
		obj->ypos-=16*65536;
		PutWalkerOnGround(num);
		//obj->ypos-=11*65536;
		obj->xpos-=16*65536;

		obj->state=sSTILL;
	}
	else
	{
		obj->counter++;
		if (obj->counter>7)
		{
			obj->counter=0;
			obj->phase++;
			if (obj->phase>=(int)anims[obj->curanim].numframes)
				obj->phase=0;

			obj->curframe=anims[obj->curanim].framelist[obj->phase];
		};	

		AddSprite(obj->xpos,obj->ypos,SPRITELAYER,0,spriteNORMAL,0,obj->curframe);

		px=obj->xorg/(32*65536);
		py=obj->yorg/(32*65536);


		for (nump=0;nump<MAXPLAYERS;nump++)
		if (player[nump].Active)
		{
			play=&player[nump];

			dx=(play->xpos-obj->xpos)/65536;
			dy=(play->ypos-obj->ypos)/65536;

			calc=dx*dx+dy*dy;
			if ((calc<32*32) && (play->warpcounter<=0))
			{
				check=GetEventParm(px,py,8,8);

				if (play->gem[0]>=check)
				{
					if (check)
					{
						play->gem[0]-=check; //add anim or so

						calc=FindEmptyMessage(nump,4);
						play->Display[calc].State=4;	//show coins
						play->Display[calc].Var1=0;	//show coins

						level.changecoins++; //got warped!
					}

					warpid=GetEventParm(px,py,0,8);
					play->warparea=1+warpid;

					DumpBuf=Buffer16K;
					numevent=CountEventParm(areaWARPTARGET,warpid,DumpBuf);
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
						sprintf(play->Display[calc].Text,"x%u",check-play->gem[0]);
					}	//check
				}
			}
		}	//for nump

	}
} //cWARP()



void bumbee(int num)
{
	Omonster *obj;
	int oldy;
	Word8 oldfreeze;
	int nump;
//	int wantx,wanty;
	int wantsx,wantsy,maxspeed;

	obj=(Omonster *)(&gameobj[num]);


	if (obj->state!=sFREEZE)
		obj->channel=PlayLoopSample(obj->xpos,obj->ypos,sBUMBEE_BEELOOP,43,22050,obj->channel);
	
	switch (obj->state)
	{
	case sSTART:
		obj->state=sIDLE;
		break;

	case sIDLE:
		obj->counter++;

		obj->xpos=obj->xorg+sintable(obj->counter*8)*16;
		if ((obj->counter&127)<63)
			obj->direction=-1;
		else
			obj->direction=+1;
		obj->ypos=obj->yorg+
					costable(obj->counter*8)*16+
					sintable(gamecounter*8)*4;

		if (obj->ypos>level.waterlevel-12*65536) obj->ypos=level.waterlevel-12*65536;

		obj->phase=(Word8)((gamecounter/6)%anims[obj->curanim].numframes);
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);

		nump=GetNearestPlayer(num,256*256);
		if (nump>=0)
		{	
			if (player[nump].xpos<obj->xpos)
				obj->direction=-1;
			else
			if (player[nump].xpos>obj->xpos)
				obj->direction=1;

			obj->state=sATTACK;
			obj->counter=AISPEED*2;
			obj->var1=nump;
		};				
		break;


	case sATTACK:
		wantsx=(player[obj->var1].xpos-obj->xpos);

		wantsy=(player[obj->var1].ypos-obj->ypos);

		maxspeed=(2+level.difficulty)*65536;

		if (wantsx>maxspeed)
			wantsx=maxspeed;
		else
		if (wantsx<-maxspeed)
			wantsx=-maxspeed;

		if (wantsy>maxspeed)
			wantsy=maxspeed;
		else
		if (wantsy<-maxspeed)
			wantsy=-maxspeed;

		if (obj->xspeed<wantsx)
			obj->xspeed+=1024*4;
		else
		if (obj->xspeed>wantsx)
			obj->xspeed-=1024*4;
		else
			obj->xspeed=wantsx;


		if (obj->yspeed<wantsy)
			obj->yspeed+=1024*4;
		else
		if (obj->yspeed>wantsy)
			obj->yspeed-=1024*4;
		else
			obj->yspeed=wantsy;

		obj->xpos+=obj->xspeed;
		obj->ypos+=obj->yspeed;		

		if (obj->ypos>level.waterlevel-12*65536) obj->ypos=level.waterlevel-12*65536;

		if (obj->xspeed<0) obj->direction=-1;
		else
		if (obj->xspeed>0) obj->direction=1;

		obj->phase=(Word8)((gamecounter/6)%anims[obj->curanim].numframes);
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);

		obj->counter--;
		if (obj->counter<0)
		{
			obj->state=sFLY;	//away
			obj->counter=0;
			obj->var2=64;
			obj->var3=(obj->xorg-obj->xpos)/64;
			obj->var4=(obj->yorg-obj->ypos)/64;
		};
		break;

	case sFLY: //away
		if (obj->var3<-32768)
			obj->direction=-1;
		else
			obj->direction=1;

		obj->xpos+=obj->var3;
		obj->ypos+=obj->var4;
		if (obj->ypos>level.waterlevel-12*65536) obj->ypos=level.waterlevel-12*65536;

		obj->var2--;
		if (obj->var2<=0)
		{
			obj->state=sIDLE;
			obj->counter=0;
			//obj->var2=0; //already
		};
		obj->phase=(Word8)((gamecounter/6)%anims[obj->curanim].numframes);
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;

	case sFREEZE:
		if (obj->freeze>0) obj->freeze--;
		if (obj->freeze==0) obj->state=obj->oldstate;

		if (obj->freeze<30)
		{
			oldy=obj->ypos;		obj->ypos+=4*Rand2Fac(16383);
			if (obj->ypos>level.waterlevel-12*65536) obj->ypos=level.waterlevel-12*65536;

			oldfreeze=obj->freeze;	obj->freeze=(Word8)RandFac(1); //let freeze anim flicker

			DrawGameobj(num);
			obj->freeze=oldfreeze;
            obj->ypos=oldy;
		} else
		DrawGameobj(num);

		break;

	case sKILL:
		cKILL(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;

	default:
		FAIL("bumbee(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //switch
} //bumbee()






void cPLASMAWITCH(int num) //bullet
{
	Obullets	*obj;
	int			dx,dy;
	int			len;


	obj=(Obullets *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		obj->force=1;
		obj->objtype=oENEMYBULLET;
		obj->curanim=AnimBase[mWITCH]+mWITCH_PLASMA;
		obj->killanim=AnimBase[mAMMO]+mAMMO_BOOM3;
		obj->lighttype=2;
		obj->phase=0;
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		obj->state=sFLY;
		//break;


	case sFLY:

		obj->var2++;
		if (obj->var2>2*AISPEED)
		{
			obj->state=sEXPLODE;
			cEXPLODE(num);
			return;
		}

		if ((obj->var2&3)==3)
		{
			if (obj->var1>=PLAYSLOT)
			{
				dx=(player[obj->var1-PLAYSLOT].xpos-obj->xpos)/65536;
				dy=(player[obj->var1-PLAYSLOT].ypos-obj->ypos)/65536;
			}
			else
			{
				dx=(gameobj[obj->var1].xpos-obj->xpos)/65536;
				dy=(gameobj[obj->var1].ypos-obj->ypos)/65536;
			}

			len=(int)sqrt(dx*dx+dy*dy);

			if (len>0)
			{
				len=4*65536/len;
				dx=dx*len;
				dy=dy*len;

				obj->xspeed=dx;
				obj->yspeed=dy;

				if (dx>0)
					obj->direction=1;
				else
				if (dx<0)
					obj->direction=-1;
			}
		}


		obj->xpos+=obj->xspeed;
		obj->ypos+=obj->yspeed;

		obj->counter++;
		if (obj->counter>3)
		{
			obj->counter=0;
			obj->phase++;
			if (obj->phase>=(int)anims[obj->curanim].numframes)
				obj->phase=0;

			obj->curframe=anims[obj->curanim].framelist[obj->phase];
		}
		DrawGameobj(num);
		break;

	case sEXPLODE:
		obj->freeze=0;
		cEXPLODE(num);
		break;

	case sKILL:
	case sDEACTIVATE:
		MyDeleteObject(num);
		break;

	default:
		FAIL("cPLASMAWITCH(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	} //switch
} //cPLASMAWITCH()



#define WITCHSPEED (2*65536)
void cWITCH(int num)
{
	Omonster	*obj;
	int			nump;
	int			dx,dy;
	int			res;
	Obullets	*bobj;


	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		obj->state=sIDLE;
		//break;

	case sIDLE:

		obj->var4+=4;

		obj->xpos=obj->xorg+96*sintable(obj->var4);
		if ((obj->var4&1023)<512)
			obj->direction=1;
		else
			obj->direction=-1;

		obj->ypos=obj->yorg+64*costable(obj->var4);

		nump=GetNearestPlayer(num,2*256*256);

		if (nump>=0)
		{
			obj->state=sFLY;

			obj->curanim=AnimBase[mWITCH]+mWITCH_FLY;

			obj->var1=PLAYSLOT+nump;
			obj->var2=0; //firespeed
		}

		obj->counter++;
		if (obj->counter>6)
		{
			obj->phase++;
			obj->counter=0;
			if (obj->phase>=(int)anims[obj->curanim].numframes)
				obj->phase=0;
		}
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;


	case sFLY:
		if (obj->var1<PLAYSLOT)
			obj->var1=PLAYSLOT;	//just to be sure

		if (player[obj->var1-PLAYSLOT].character==mFROG)
		{
			//PlaySample(HAHAHAHA);
			PlaySample(obj->xpos,obj->ypos,sWITCH_LAUGH,0,0);
			obj->state=sFADEOUT;
			obj->var2=0; //no attack
		}

		dx=player[obj->var1-PLAYSLOT].xpos-obj->xpos+64*sintable(gamecounter);
		dy=player[obj->var1-PLAYSLOT].ypos-96*65536-obj->ypos; //target above player

		if (dx>16*32768) dx=16*32768;
		else
		if (dx<-16*32768) dx=-16*32768;

		if (dy>8*32768) dy=8*32768;
		else
		if (dy<-8*32768) dy=-8*32768;

		obj->xspeed=(dx+obj->xspeed)/4;
		obj->yspeed=(dy+obj->yspeed)/4;


		if (obj->xspeed>0)
			obj->direction=1;
		else
		if (obj->xspeed<0)
			obj->direction=-1;


		obj->xpos+=obj->xspeed;
		obj->ypos+=obj->yspeed;

		obj->var2++;

		if (obj->ypos>obj->yorg+96*65536)
			obj->ypos=obj->yorg+96*65536;
		else	//dont fire!
		if (obj->var2>AISPEED*2)
		{
			obj->state=sATTACK;
			obj->phase=0;
			obj->curanim=AnimBase[mWITCH]+mWITCH_ATTACK;
			obj->var2=0;
		}

		obj->counter++;
		if (obj->counter>6)
		{
			obj->phase++;
			obj->counter=0;
			if (obj->phase>=(int)anims[obj->curanim].numframes)
				obj->phase=0;
		}
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;


	case sATTACK:
		obj->counter++;
		if (obj->counter>4)
		{
			obj->phase++;
			obj->counter=0;
			if (obj->phase>=(int)anims[obj->curanim].numframes)
			{
				obj->phase=0;
				obj->curanim=AnimBase[mWITCH]+mWITCH_FLY;
				obj->state=sFLY;
			}
			else
			if (obj->phase==9)
			{
				res=AddBullet(num,aBULLET);

				if (res>=0)
				{	//PLASMABULLET, fill it in completely!
					bobj=(Obullets *)(&gameobj[res]);
					bobj->ProcPtr=&cPLASMAWITCH;

					bobj->var1=obj->var1;
					bobj->lifetime=AISPEED/2;
				}
				PlaySample(obj->xpos,obj->ypos,sWITCH_MAGIC,0,0);
			}
		}
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;


	case sFADEOUT:
		//fly away

		if (obj->direction<0)
			obj->xspeed-=16384;
		else
			obj->xspeed+=16384;

		obj->xpos+=obj->xspeed;
		
		obj->counter++;
		if (obj->counter>6)
		{
			obj->phase++;
			obj->counter=0;
			if (obj->phase>=(int)anims[obj->curanim].numframes)
				obj->phase=0;
		}

		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;


	case sFREEZE:
        if (obj->freeze>0) obj->freeze--;
        if (obj->freeze==0) obj->state=obj->oldstate;
		DrawGameobj(num);
		break;

	case sKILL:
		cKILL(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;

	default:
		FAIL("cWITCH(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	} //switch

} //cWITCH()




void cBUTTERFLY(int num)
{
	Omonster	*obj;
//	int			nump;
//	int			dx,dy;


	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		obj->state=sFLY;
		//break;

	case sFLY:

		//4 frames going up
		//3 frames turning left
		//4 frames going left
		//3 frames turning down


		break;

	case sFREEZE:
        if (obj->freeze>0) obj->freeze--;
        if (obj->freeze==0) obj->state=obj->oldstate;
		DrawGameobj(num);
		break;

	case sKILL:
		cKILL(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;

	default:
		FAIL("cBUTTERFLY(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	} //switch

}	//cBUTTERFLY()




void cBEEBOY(int num)
{
	Omonster *obj;
	int oldy;
	Word8 oldfreeze;
	int nump;
//	int wantx,wanty;
	int wantsx,wantsy,maxspeed;
	int t;
	int res;
	Omonster *other;
	int dx,dy,dist; 
	int dxs,dys;
	int newx,newy;
	int numbirds;
	int mindist=3*16;
	int maxdist=256;	
	obj=(Omonster *)(&gameobj[num]);

	
	//check which bee is the leader
	if (obj->state!=sSTART)
	{
		obj->var6=num;
		for (t=0;t<NumObjects;t++)
		if (gameobj[t].ProcPtr==&cBEEBOY)
		{
			obj->var6=t;
			break;
		}	//for t

		if (obj->var6==num)
		{
			if (obj->state==sEXTRA)
				obj->state=sIDLE;
		}
	}	//sSTART


	switch (obj->state)
	{
	case sSTART:
		obj->state=sIDLE;

		if (obj->creator==LEVELSLOT)
		{
			obj->var1=4;

			for (t=0;t<obj->var1;t++)
			{
				res=AddObject(obj->xpos,obj->ypos,aBEEBOY,num);
				if (res>0)
				{
					other=(Omonster *)(&gameobj[res]);
					other->xpos+=4*16*RandFac(32767);
					other->ypos+=4*16*RandFac(32767);
					other->xspeed=Rand2Fac(32767)*4;
					other->yspeed=Rand2Fac(32767)*2;
					other->state=sEXTRA;
					//gameobj[res].var2=RandFac(1023);	//angle
				}
			}

			obj->creator=num;	//
			obj->state=sIDLE;
		}	//only if level object trigger MORE of them bees!
		else
			obj->state=sEXTRA;
		break;


	case sEXTRA:

		if (obj->var6!=num)
		{
			other=(Omonster *)(&gameobj[obj->var6]);

			obj->xspeed=(obj->xspeed+other->xspeed)/2;
			obj->yspeed=(obj->yspeed+other->yspeed)/2;
		}

		newx=obj->xpos+obj->xspeed;
		newy=obj->ypos+obj->yspeed;

		numbirds=1;

		for (t=0;t<NumObjects;t++)
		if ((gameobj[t].ProcPtr==&cBEEBOY) && (t!=num))
		{
			other=(Omonster *)(&gameobj[t]);

			dx=other->xpos-obj->xpos;
			dxs=dx/65536;
			dy=other->ypos-obj->ypos;
			dys=dy/65536;

			dist=dxs*dxs+dys*dys;
			dist=(int)sqrt(dist);

			if (dist<mindist)
			{
				int tdist;

				tdist=mindist-dist+1;	//

				newx+=obj->xpos-(tdist*dx)/mindist;
				newy+=obj->ypos-(tdist*dy)/mindist;
				numbirds++;
			}
			else
			if (dist<mindist+8) {
			} else if (dist<maxdist) {
				int force;

				force=maxdist-(dist-mindist-8);

				newx+=obj->xpos+(dx*force)/maxdist;
				newy+=obj->ypos+(dy*force)/maxdist;

				numbirds++;
			}
		}	//for t

		newx=newx/numbirds;
		newy=newy/numbirds;

		if (newx<obj->xpos-65536)
			obj->direction=-1;
		else
		if (newx>obj->xpos+65536)
			obj->direction=1;

		obj->xpos=newx;
		obj->ypos=newy;
/*
		dx=(obj->xpos-obj->xorg)/65536;
		if (dx<-100) obj->xpos=obj->xorg-100*65536;
		else
		if (dx>100) obj->xpos=obj->xorg+100*65536;

		dy=(obj->ypos-obj->yorg)/65536;
		if (dy<-100) obj->ypos=obj->yorg-100*65536;
		else
		if (dy>100) obj->ypos=obj->yorg+100*65536;
*/

		obj->phase=(Word8)((gamecounter/6)%anims[obj->curanim].numframes);
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;


	case sIDLE:
		obj->counter++;

		obj->xpos=obj->xorg+sintable(obj->counter*8)*16;
		if ((obj->counter&127)<63)
			obj->direction=-1;
		else
			obj->direction=+1;
		obj->ypos=obj->yorg+
					costable(obj->counter*8)*16+
					sintable(gamecounter*8)*4;

		if (obj->ypos>level.waterlevel-12*65536) obj->ypos=level.waterlevel-12*65536;

		obj->phase=(Word8)((gamecounter/6)%anims[obj->curanim].numframes);
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);

		nump=GetNearestPlayer(num,256*256);
		if (nump>=0)
		{	
			if (player[nump].xpos<obj->xpos)
				obj->direction=-1;
			else
			if (player[nump].xpos>obj->xpos)
				obj->direction=1;

			obj->state=sATTACK;
			obj->counter=AISPEED*2;
			obj->var1=nump;
		};				
		break;


	case sATTACK:
		wantsx=(player[obj->var1].xpos-obj->xpos);

		wantsy=(player[obj->var1].ypos-obj->ypos);

		maxspeed=(2+level.difficulty)*65536;

		if (wantsx>maxspeed)
			wantsx=maxspeed;
		else
		if (wantsx<-maxspeed)
			wantsx=-maxspeed;

		if (wantsy>maxspeed)
			wantsy=maxspeed;
		else
		if (wantsy<-maxspeed)
			wantsy=-maxspeed;

		if (obj->xspeed<wantsx)
			obj->xspeed+=1024*4;
		else
		if (obj->xspeed>wantsx)
			obj->xspeed-=1024*4;
		else
			obj->xspeed=wantsx;


		if (obj->yspeed<wantsy)
			obj->yspeed+=1024*4;
		else
		if (obj->yspeed>wantsy)
			obj->yspeed-=1024*4;
		else
			obj->yspeed=wantsy;

		obj->xpos+=obj->xspeed;
		obj->ypos+=obj->yspeed;		

		if (obj->ypos>level.waterlevel-12*65536) obj->ypos=level.waterlevel-12*65536;

		if (obj->xspeed<0) obj->direction=-1;
		else
		if (obj->xspeed>0) obj->direction=1;

		obj->phase=(Word8)((gamecounter/6)%anims[obj->curanim].numframes);
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);

		obj->counter--;
		if (obj->counter<0)
		{
			obj->state=sFLY;	//away
			obj->counter=0;
			obj->var2=64;
			obj->var3=(obj->xorg-obj->xpos)/64;
			obj->var4=(obj->yorg-obj->ypos)/64;
		};
		break;

	case sFLY: //away
		if (obj->var3<-32768)
			obj->direction=-1;
		else
			obj->direction=1;

		obj->xpos+=obj->var3;
		obj->ypos+=obj->var4;
		if (obj->ypos>level.waterlevel-12*65536) obj->ypos=level.waterlevel-12*65536;

		obj->var2--;
		if (obj->var2<=0)
		{
			obj->state=sIDLE;
			obj->counter=0;
			//obj->var2=0; //already
		};
		obj->phase=(Word8)((gamecounter/6)%anims[obj->curanim].numframes);
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;


	case sFREEZE:
		if (obj->freeze>0) obj->freeze--;
		if (obj->freeze==0) obj->state=obj->oldstate;

		if (obj->freeze<30)
		{
			oldy=obj->ypos;		obj->ypos+=4*Rand2Fac(16383);
			if (obj->ypos>level.waterlevel-12*65536) obj->ypos=level.waterlevel-12*65536;

			oldfreeze=obj->freeze;	obj->freeze=(Word8)RandFac(1); //let freeze anim flicker

			DrawGameobj(num);
			obj->freeze=oldfreeze;
            obj->ypos=oldy;
		} else
		DrawGameobj(num);
		break;

	case sKILL:
		cKILL(num);
		break;

	case sDEACTIVATE:
		cDEACTIVATE(num);
		break;

	default:
		FAIL("cBEEBOY(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //switch
} //cBEEBOY()




static int LastSnowed;

void cSNOW(int num)
{
	Omonster *obj;
	int nump;
	int x,y;


	obj=(Omonster *)(&gameobj[num]);
	//always active during a level, adds particles every frame


	if (LastSnowed==(int)gamecounter)
	{
		//double object??
		DebugXMessage("Double SNOW event in level??");
		MyDeleteObject(num);
		return;
	}
	else
		LastSnowed=gamecounter;


	obj->state=sFLY;
	obj->curframe=0;

	for (nump=0;nump<(int)NumPlayers;nump++)
	if (player[nump].localplayer)
	{
		x=player[nump].viewstartx+player[nump].xspeed-32*65536+RandFac(32767)*2*(ScreenWidth+64);
		y=player[nump].viewstarty-32*65536+RandFac(32767)*2*ScreenHeight;
		AddParticleSnow(x,y);
	}

}	//cSNOW()





void cFLAG(int num)
{
	Omonster *obj;
	int n;
	int d,t,dx,dy,dist,nearest;
	int x,y;
	char s[128];

	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		obj->var2=GetEventParm(obj->xorg/(32*65536),obj->yorg/(32*65536),0,1);
		obj->var3=GetEventParm(obj->xorg/(32*65536),obj->yorg/(32*65536),1,1);
		obj->state=sSLEEP;
		obj->var7=obj->var1;
		obj->var8=obj->state;
		obj->var5=0;
		break;

	case sFLY:
		if ((obj->var1>=PLAYSLOT) && (obj->var1<PLAYSLOT+MAXPLAYERS))
		{
			if (player[obj->var1-PLAYSLOT].Active)
			{
				obj->direction=-player[obj->var1-PLAYSLOT].direction;

				obj->xpos=player[obj->var1-PLAYSLOT].xpos;
				obj->ypos=player[obj->var1-PLAYSLOT].ypos;
			}
		}
		break;

	case sSLEEP:
		obj->state=sSLEEP;
		obj->xpos=obj->xorg;
		obj->ypos=obj->yorg+15*65536;

		if (((MultiPlayerConnection>gameLOCAL) && LocalComputerIsServer) ||
			(MultiPlayerConnection==gameLOCAL))
		{


//find nearest flag
			n=GetNearestObject(obj->xpos,obj->ypos,48*48,aFLAG,sFLY);
			if (n>0)
			{
				Omonster *obj2;			//ottah flag
				obj2=(Omonster *)(&gameobj[n]);
				obj2->state=sSLEEP;	//return ottah flag to base
				obj2->xpos=obj2->xorg;
				obj2->ypos=obj2->yorg+15*65536;

				//if (obj2->var2
				//team dragged another flag to this base, give the players at this base points
				
				TeamScores[obj->var2]+=1;

				ChatAddString(EStringGetPtr(strBLUESCORED+obj->var2));
				EStringUnlock();

				if (TeamScores[obj->var2]>=level.extra)
				{
					int res;

					for (res=0;res<MAXPLAYERS;res++)
					if (player[res].Active && player[res].localplayer && (player[res].finished==0))	//&& (player[res].Team==obj->var2) 
					{
						player[res].finished=1;	//all players are finished
					}

					if (MultiPlayerConnection>gameLOCAL)
						Server_LevelFinished(res);
				}

				//captured: send
			}	//	n>0 found another flag!!!


			n=-1;
			nearest=32*32;
			for (t=0;t<MAXPLAYERS;t++)
			if (player[t].Active && (player[t].Team!=obj->var2))
			{
				dx=(player[t].xpos-obj->xpos)/65536;
				dist=dx*dx;
				if (dist<nearest)
				{
					dy=(player[t].ypos-obj->ypos)/65536;
					dist+=dy*dy;
					if (dist<nearest)
					{
						nearest=dist;
						n=t;
					}
				}
			}

			if (n>=0)
			{
				obj->var1=n+PLAYSLOT;
				obj->state=sFLY;
				for (t=0;t<MAXPLAYERS;t++)	//clear flags
					if (player[t].Flag==num) player[t].Flag=0;

				player[n].Flag=num;
			}

		}	//only 
		break;

	case sFREEZE:
		obj->state=obj->oldstate;
		break;


	default:
		obj->var1=0;
		obj->state=sSLEEP;
		break;
	}	//state


	if ((obj->var1<PLAYSLOT) || (obj->var1>=PLAYSLOT+MAXPLAYERS) || (player[obj->var1-PLAYSLOT].Active==0))
	{
		obj->state=sSLEEP;
	}

	if (obj->state==sSLEEP)
	{
		obj->var1=0;
	}

	FlagStates[obj->var2]=obj->var1;
	FlagObjectNum[obj->var2]=num;


	if ((obj->var1!=obj->var7) || (obj->state!=obj->var8))
	{
		if ((obj->var1!=obj->var7) && LocalComputerIsServer && obj->var1)
		{
			sprintf(s,EStringGetPtr(strMSG_CAPTURED),player[obj->var1-PLAYSLOT].name);
			EStringUnlock();
			ChatAddString(s);
		}

		obj->var7=obj->var1;
		obj->var8=obj->state;
	}	//state change



//BASE!!
	if (obj->var3)
		d=1;
	else
		d=-1;
	x=obj->xorg+5*65536*d;
	y=obj->yorg+17*65536;
	obj->curframe=anims[AnimBase[mFLAG]+mFLAG_BASE].framelist[obj->var2];
	AddSprite(x,y,SPRITELAYER,d,spriteNORMAL,0,obj->curframe);

	x=obj->xorg+23*65536*d;
	y=obj->yorg-36*65536;
	obj->curanim=AnimBase[mFLAG]+mFLAG_BASE_LED;
	obj->phase=(Word8)((gamecounter/6)%anims[obj->curanim].numframes);
	obj->curframe=anims[obj->curanim].framelist[obj->phase];
	AddSprite(x,y,SPRITELAYER,d,spriteNORMAL,0,obj->curframe);


	x=obj->xorg-64*65536*d;
	y=obj->yorg-4*65536;
	if (obj->var5==0)
	{
		obj->curanim=AnimBase[mFLAG]+mFLAG_EVA_IDLE;
		obj->phase=(Word8)((gamecounter/6)%anims[obj->curanim].numframes);
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		AddSprite(x,y,SPRITELAYER,-d,spriteNORMAL,0,obj->curframe);
	}
	else
	{
		obj->curanim=AnimBase[mFLAG]+mFLAG_EVA_WIN;
		obj->counter++;
		obj->phase=obj->counter/4;
		if (obj->phase>=(int)anims[obj->curanim].numframes)
		{
			obj->phase=anims[obj->curanim].numframes-1;
			obj->var5=0;
		}
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		AddSprite(x,y,SPRITELAYER,-d,spriteNORMAL,0,obj->curframe);
	}


//FLAG!
	obj->freeze=0;
	if (obj->var2==0)
		obj->curanim=AnimBase[mFLAG]+mFLAG_BLUE;
	else
		obj->curanim=AnimBase[mFLAG]+mFLAG_RED;
	obj->phase=(Word8)((gamecounter/6)%anims[obj->curanim].numframes);
	obj->curframe=anims[obj->curanim].framelist[obj->phase];
	AddSprite(obj->xpos,obj->ypos+6*65536,SPRITELAYER,obj->direction,spriteNORMAL,0,obj->curframe);



}	//cFLAG()
