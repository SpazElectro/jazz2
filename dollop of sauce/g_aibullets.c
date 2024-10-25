/*
	G_AIBULLETS.C


*/
#include "G_Ai.h"
#include "Random.h"
#include "E_Graphics.h"
#include "E_Loaders.h"
#include "E_Sound.h"
#include "Globals.h"
#include "G_Objects.h"
#include "G_AiTypes.h"
#include "G_Collision.h"
#include "E_Particles.h"
#include "E_Parallax.h"
#include "G_Bullet.h"
#include "Grade.h"
#include "G_Hit.h"
#include "N_Main.h"

#if _DEBUG
#define FAIL	Fatal
#else
#define FAIL	1 ? (void)0 : Fatal
#endif


static int ricosamps[4]={sAMMO_BUL1,sAMMO_BULFL1,sAMMO_BULFL2,sAMMO_BULFL3};


void cBULLET1(int num)
{
	Obullets *obj;
	int t,hit;
//	Tgameobj *parent;
	Omonster *monster;
//	int pdx,pdy;
	int res;


	obj=(Obullets *)(&gameobj[num]);


	if (obj->state==sSTART)
	{
		if (obj->creator>=PLAYSLOT)
		{	//CREATED BY PLAYER!
//			obj->direction=player[obj->creator-PLAYSLOT].direction;
//			obj->xspeed=(obj->direction*obj->xspeed)+player[obj->creator-PLAYSLOT].xspeed;
//			obj->xacc=(obj->direction*obj->xacc);
		}
		else
		{	//CREATED BY MONSTER
			monster=(Omonster *)(&gameobj[obj->creator]);
			obj->direction=monster->direction;
			obj->xspeed=(obj->direction*obj->xspeed)+monster->xspeed;
			obj->xacc=(obj->direction*obj->xacc);
		}

		obj->state=sFLY;
	}
	else
	if (obj->state==sEXPLODE)
	{
		obj->freeze=0;
		cEXPLODE(num);
		return;
	}
	else
	if ((obj->state==sKILL) || (obj->state==sDEACTIVATE))
	{
		MyDeleteObject(num);//obj->code=NULL;
		return;
	}

	obj->xpos+=obj->xspeed+obj->pxspeed;
	obj->ypos+=obj->yspeed;//+obj->pyspeed;


	if (obj->pxspeed>8192)
		obj->pxspeed-=8192;
	else
	if (obj->pxspeed<-8192)
		obj->pxspeed+=8192;
	else
		obj->pxspeed=0;



	if (obj->counter>obj->lifetime)
	{
		if (obj->load==aPLAYERBULLETC3)
			PlaySample(obj->xpos,obj->ypos,sCOMMON_ELECTRICHIT,80,0);
		else
			PlaySample(obj->xpos,obj->ypos,sCOMMON_EXPSM1,80,0);

		obj->state=sEXPLODE;
		obj->phase=0;
		obj->counter=0;
		obj->lighttype=0;
	}



	obj->xspeed+=obj->xacc;
	if (obj->xspeed>10*65536)
		obj->xspeed=10*65536;
	else
	if (obj->xspeed<-10*65536)
		obj->xspeed=-10*65536;

	obj->yspeed+=obj->yacc;


	hit=CheckPixel(obj->xpos,obj->ypos);

	if (hit==1)
	{
		if ((tileattr==areaRICOCHET) && (obj->lastrico>4) && (obj->ricos<8))//ricochet
		{
			obj->xpos-=obj->xspeed+obj->pxspeed; //to move it away
			obj->xspeed=-obj->xspeed;
			obj->xacc=-obj->xacc;
			obj->direction=-obj->direction;
			obj->yspeed=-obj->yspeed+Rand2Fac(32767)*16;
			if (obj->yspeed<-8*65536) obj->yspeed=-8*65536;
			else
			if (obj->yspeed>8*65536) obj->yspeed=8*65536;

			for (t=0;t<8+(int)RandFac(7);t++)
				AddParticleSparks(obj->xpos,obj->ypos,-obj->direction);

			PlaySample(obj->xpos,obj->ypos,ricosamps[RandFac(3)],0,0);
			obj->ricos++;
			obj->lastrico=0;
		}
		else
		{
			if (obj->load==aPLAYERBULLETC3)
				PlaySample(obj->xpos,obj->ypos,sCOMMON_ELECTRICHIT,100,0);
			else
				PlaySample(obj->xpos,obj->ypos,sCOMMON_EXPSM1,100,0);
			obj->state=sEXPLODE;
			obj->phase=0;
			obj->counter=0;
			obj->lighttype=0;
		}
	}
	obj->lastrico++;

	if (hit==-2)
		obj->state=sKILL;


	if (obj->state==sEXPLODE)	//exploding!
	{
		if (obj->load==aPLAYERBULLETC3)
		{	//electric shield

			if (hit!=1)
				PlaySample(obj->xpos,obj->ypos,sCOMMON_ELECTRIC2,0,0);

			t=RandFac(3);
			if (t)
			do
			{
				res=AddObject(obj->xpos,obj->ypos,aSHARD,0);
				if (res>0)
				{
					gameobj[res].curanim=AnimBase[mAMMO]+mAMMO_SPARK1;
					gameobj[res].light=1;
					gameobj[res].lighttype=2;
				} else break;
			} while (--t);
		}
		else
		if (obj->load==aPLAYERBULLETC1)
		{
			t=RandFac(3);
			if (t)
			do
			{
				res=AddObject(obj->xpos,obj->ypos,aBOUNCEONCE,0);
				if (res>0)
				{
					gameobj[res].curanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
					gameobj[res].light=1;
					gameobj[res].lighttype=2;
				} else break;
			} while (--t);
		}
	}


	obj->counter++;
	if (obj->counter<3) //DONT DISPLAY THE BULLET WHEN STILL INSIDE CHARACTER!!
		return;

	if ((obj->counter&3)==0)
	{
		obj->phase++;
		if (obj->phase>=anims[obj->curanim].numframes)
			obj->phase=0;
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
	}

	DrawGameobj(num);

	if (obj->load==aPLAYERBULLET3)
	{
		if ((!RandFac(3)) && (obj->ypos<level.waterlevel))
			AddParticlePixel(obj->xpos,obj->ypos,4);
	} else if (obj->load==aPLAYERBULLETC3) {
		if (NumPlayers==1)
			t=((obj->counter&7)==0);
		else
			t=((obj->counter&15)==0);

		if (t)
			AddExplosion(obj->xpos,obj->ypos,AnimBase[mAMMO]+mAMMO_ELETRAIL); //elecparticle
	}

} //cBULLET1



void cSINEBULLET1(int num)
{
	Obullets *obj;
	int t,hit;
//	Tgameobj *parent;
	Omonster *monster;
//	int pdx,pdy;


	obj=(Obullets *)(&gameobj[num]);

	if (obj->state==sSTART)
	{
		if (obj->creator>=PLAYSLOT)
		{	//CREATED BY PLAYER!
//			obj->direction=player[obj->creator-PLAYSLOT].direction;
//			obj->xspeed=(obj->direction*obj->xspeed)+player[obj->creator-PLAYSLOT].xspeed;
//			obj->xacc=(obj->direction*obj->xacc);
		}
		else
		{	//CREATED BY MONSTER
			monster=(Omonster *)(&gameobj[obj->creator]);
			obj->direction=monster->direction;
			obj->xspeed=(obj->direction*obj->xspeed)+monster->xspeed;
			obj->xacc=(obj->direction*obj->xacc);
		}

//		obj->ypos+=costable(obj->var1)*4;
		obj->state=sFLY;
	}
	else
	if (obj->state==sEXPLODE)
	{
		obj->freeze=0;
		cEXPLODE(num);
		return;
	}
	else
	if ((obj->state==sKILL) || (obj->state==sDEACTIVATE))
	{
		MyDeleteObject(num);//obj->code=NULL;
		return;
	}

	if (obj->xspeed==0)
	{
		obj->xpos+=sintable(obj->var1)+obj->pxspeed;
		obj->ypos+=obj->yspeed;
	}
	else
	{
		obj->xpos+=obj->xspeed+obj->pxspeed;
		obj->ypos+=sintable(obj->var1);
	}

	if (obj->pxspeed>8192)
		obj->pxspeed-=8192;
	else
	if (obj->pxspeed<-8192)
		obj->pxspeed+=8192;
	else
		obj->pxspeed=0;

	obj->var1+=32*2;


	if (obj->counter>obj->lifetime)
	{
		PlaySample(obj->xpos,obj->ypos,sCOMMON_EXPSM1,58,0);
		obj->state=sEXPLODE;
		obj->phase=0;
		obj->counter=0;
		obj->lighttype=0;
	}

	obj->xspeed+=obj->xacc;
	obj->yspeed+=obj->yacc;

	hit=CheckPixel(obj->xpos,obj->ypos);
	if (hit==1)
	{
		if ((tileattr==areaRICOCHET) && (obj->lastrico>4) && (obj->ricos<8))//ricochet
		{
			obj->xpos-=obj->xspeed; //to move it away
			obj->xspeed=-obj->xspeed;
			obj->xacc=-obj->xacc;
			obj->direction=-obj->direction;
			obj->yspeed=-obj->yspeed+Rand2Fac(32767)*16;
			if (obj->yspeed<-8*65536) obj->yspeed=-8*65536;
			else
			if (obj->yspeed>8*65536) obj->yspeed=8*65536;

			for (t=0;t<8+(int)RandFac(7);t++)
				AddParticleSparks(obj->xpos,obj->ypos,-obj->direction);

			PlaySample(obj->xpos,obj->ypos,ricosamps[RandFac(3)],0,0);
			obj->lastrico=0;
			obj->ricos++;
		}
		else
		{
			PlaySample(obj->xpos,obj->ypos,sCOMMON_EXPSM1,58,0);
			obj->state=sEXPLODE;
			obj->phase=0;
			obj->counter=0;
			obj->lighttype=0;
		}
	}
	obj->lastrico++;

	if (hit==-2)
		obj->state=sKILL;

	if ((obj->counter&3)==0)
	{
		obj->phase++;
		if (obj->phase>=anims[obj->curanim].numframes)
			obj->phase=0;
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
	}

	obj->counter++;
	if (obj->counter<4) //DONT DISPLAY INSIDE CHAR
		return;

	DrawGameobj(num);


	if (obj->load==aPLAYERBULLETP3)
	{
		if ((!RandFac(1)) && (obj->ypos<level.waterlevel))
			AddParticlePixel(obj->xpos,obj->ypos,4);
	}

} //cSINEBULLET1






void cROCKETFLY(int num)
{
	Obullets *obj;
	int t,hit;
//	Tgameobj *parent;
//	int pdx,pdy;

	obj=(Obullets *)(&gameobj[num]);

	obj->xpos+=obj->xspeed+obj->pxspeed;
	obj->ypos+=obj->yspeed;

	if (obj->pxspeed>8192)
		obj->pxspeed-=8192;
	else
	if (obj->pxspeed<-8192)
		obj->pxspeed+=8192;
	else
		obj->pxspeed=0;


	obj->xspeed+=obj->xacc;
	obj->yspeed+=obj->yacc;


	if (obj->counter>obj->lifetime)
	{
		obj->state=sEXPLODE;
		obj->phase=0;
		obj->counter=0;
		obj->lighttype=0;
	}
/*
	//destroy bullets when out of range
	if (obj->creator>=PLAYSLOT)
	{
		parent=(Tgameobj *)&player[obj->creator-PLAYSLOT];
		pdx=ABS(obj->xpos-parent->xpos);
		pdy=ABS(obj->ypos-parent->ypos);

		if ((pdx>300*65536) || (pdy>150*65536))
		{
			obj->state=sEXPLODE;
			obj->phase=0;
		}
	}
	else
	{
		parent=&gameobj[obj->creator];
		pdx=ABS(obj->xpos-parent->xpos);
		pdy=ABS(obj->ypos-parent->ypos);
		if ((parent->state==sKILL) || (pdx>300*65536) || (pdy>150*65536))
		{
			obj->state=sEXPLODE;
			obj->phase=0;
			obj->counter=0;
		}
	}
*/


	hit=CheckPixel(obj->xpos,obj->ypos);
	if (hit==1)
	{
		if ((tileattr==areaRICOCHET) && (obj->lastrico>4) && (obj->ricos<8))//ricochet
		{
			obj->xpos-=obj->xspeed+obj->pxspeed; //to move it away
			obj->xspeed=-obj->xspeed;
			obj->xacc=-obj->xacc;
			obj->direction=-obj->direction;
			obj->yspeed=-obj->yspeed+Rand2Fac(32767)*16;
			if (obj->yspeed<-8*65536) obj->yspeed=-8*65536;
			else
			if (obj->yspeed>8*65536) obj->yspeed=8*65536;

			for (t=0;t<8+(int)RandFac(7);t++)
				AddParticleSparks(obj->xpos,obj->ypos,-obj->direction);

			PlaySample(obj->xpos,obj->ypos,ricosamps[RandFac(3)],0,0);
			obj->ricos++;
			obj->lastrico=0;
		}
		else
		{
			obj->state=sEXPLODE;
			obj->phase=0;
			obj->counter=0;
			obj->lighttype=0;
		}
	}
	obj->lastrico++;

	if (hit==-2) 
		obj->state=sKILL;

	obj->counter++;
	if ((obj->counter&3)==0)
	{
		obj->phase++;
		if (obj->phase>=anims[obj->curanim].numframes)
			obj->phase=0;
	}
	obj->curframe=anims[obj->curanim].framelist[obj->phase];


	if (obj->counter>8)
	{
		if (NumPlayers==1)
			t=((obj->counter&7)==0);
		else
			t=((obj->counter&15)==0);

		if (t)
		{
			if (obj->ypos>level.waterlevel)
				AddObject(obj->xpos,obj->ypos,aBUBBLE,0);
			else
				AddExplosion(obj->xpos,obj->ypos,AnimBase[mAMMO]+mAMMO_POOF1); //smoke particle
		}

		DrawGameobj(num); //tho never freezed
	}

} //cROCKETFLY


void cBOUNCE(int num)
{
	Obullets *obj;
	int goup,godown,goleft,goright;
	int sample=0;
//	Tgameobj *parent;
	Omonster *monster;
//	int pdx,pdy;


	obj=(Obullets *)(&gameobj[num]);


	if (obj->state==sSTART)
	{
		if (obj->creator>=PLAYSLOT)
		{	//CREATED BY PLAYER!
//			obj->direction=player[obj->creator-PLAYSLOT].direction;
//			obj->xspeed=(obj->direction*obj->xspeed)+player[obj->creator-PLAYSLOT].xspeed;
//			obj->xacc=(obj->direction*obj->xacc);
			
		}
		else
		{	//CREATED BY MONSTER
			monster=(Omonster *)(&gameobj[obj->creator]);
			obj->direction=monster->direction;
			obj->xspeed=(obj->direction*obj->xspeed)+monster->xspeed;
			obj->xacc=(obj->direction*obj->xacc);
		};

		obj->state=sBOUNCE;
	} else
	if (obj->state==sEXPLODE)
	{
		cEXPLODE(num);
		return;
	} else
	if ((obj->state==sKILL) || (obj->state==sDEACTIVATE))
	{
		MyDeleteObject(num);//obj->code=NULL;
		return;
	};

	obj->xpos+=obj->xspeed+obj->pxspeed;
	obj->ypos+=obj->yspeed;

	if (obj->pxspeed>8192)
		obj->pxspeed-=8192;
	else
	if (obj->pxspeed<-8192)
		obj->pxspeed+=8192;
	else
		obj->pxspeed=0;

	obj->xspeed+=obj->xacc;
	if (obj->ypos>level.waterlevel)
		obj->yspeed+=obj->yacc+level.gravity/4;
	else
		obj->yspeed+=obj->yacc+level.gravity;


	if (obj->counter>obj->lifetime)
	{
		obj->state=sEXPLODE;
		obj->phase=0;
		obj->counter=0;
		obj->lighttype=0;
	}
/*
	//destroy bullets when out of range
	if (obj->creator>=PLAYSLOT)
	{
		parent=(Tgameobj *)&player[obj->creator-PLAYSLOT];
		pdx=ABS(obj->xpos-parent->xpos);
		pdy=ABS(obj->ypos-parent->ypos);

		if ((pdx>200*65536) || (pdy>150*65536))
		{
			obj->state=sEXPLODE;
			obj->phase=0;
			obj->counter=0;
			obj->lighttype=0;
		};
	} else
	{
		parent=&gameobj[obj->creator];
		pdx=ABS(obj->xpos-parent->xpos);
		pdy=ABS(obj->ypos-parent->ypos);
		if ((parent->state==sKILL) || (pdx>200*65536) || (pdy>150*65536))
		{
			obj->state=sEXPLODE;
			obj->phase=0;
			obj->counter=0;
			obj->lighttype=0;
			return;
		};
	};
*/

	if (obj->xspeed>8*65536) obj->xspeed=8*65536;
	else
	if (obj->xspeed<-8*65536) obj->xspeed=-8*65536;

	if (obj->yspeed>4*65536) obj->yspeed=4*65536;
	else
	if (obj->yspeed<-4*65536) obj->yspeed=-4*65536;


	goup=CheckPixel(obj->xpos,obj->ypos-4*65536);
	godown=CheckPixel(obj->xpos,obj->ypos+4*65536);
	goleft=CheckPixel(obj->xpos-2*65536,obj->ypos);
	goright=CheckPixel(obj->xpos+2*65536,obj->ypos);


	if ((obj->yspeed<0) && (goup==1))
	{
		obj->var1++;
		obj->yspeed=-(obj->yspeed*7)/8;
		sample++;
	} else
	if ((obj->yspeed>0) && (godown==1))
	{
		obj->var1++;

		obj->ypos-=obj->yspeed;
		obj->yspeed=-(obj->yspeed*7)/8;
		sample++;
	};

	if ((obj->xspeed<0) && (goleft==1))
	{
		obj->var1++;
		obj->xspeed=-(obj->xspeed*7)/8;
		obj->xacc=-obj->xacc;
		sample++;
	} else
	if ((obj->xspeed>0) && (goright==1))
	{
		obj->var1++;
		obj->xspeed=-(obj->xspeed*7)/8;
		obj->xacc=-obj->xacc;
		sample++;
	};


	if (sample)
		PlaySample(obj->xpos,obj->ypos,sCOMMON_BLOKPLOP,40,0);


	if (obj->var1>16)
	{
		obj->state=sEXPLODE;
		obj->phase=0;
		obj->counter=-1;
		obj->lighttype=0;
	};

	obj->counter++;

	if ((obj->counter&3)==0)
	{
		obj->phase++;
		if (obj->phase>=anims[obj->curanim].numframes)
			obj->phase=0;
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
	}	

	if (obj->counter>6)
		DrawGameobj(num); //tho never freezed

} //cBOUNCE


void cBOUNCE2(int num)
{
	Obullets *obj;
	int goup,godown,goleft,goright;
	int sample=0;
//	Tgameobj *parent;
	Omonster *monster;
//	int pdx,pdy;


	obj=(Obullets *)(&gameobj[num]);

	if (obj->state==sSTART)
	{
		if (obj->creator>=PLAYSLOT)
		{	//CREATED BY PLAYER!
//			obj->direction=player[obj->creator-PLAYSLOT].direction;
//			obj->xspeed=(obj->direction*obj->xspeed)+player[obj->creator-PLAYSLOT].xspeed;
//			obj->xacc=(obj->direction*obj->xacc);
			
		}
		else
		{	//CREATED BY MONSTER
			monster=(Omonster *)(&gameobj[obj->creator]);
			obj->direction=monster->direction;
			obj->xspeed=(obj->direction*obj->xspeed)+monster->xspeed;
			obj->xacc=(obj->direction*obj->xacc);
		};

		obj->state=sBOUNCE;
	} else
	if (obj->state==sEXPLODE)
	{
		cEXPLODE(num);
		return;
	} else
	if ((obj->state==sKILL) || (obj->state==sDEACTIVATE))
	{
		MyDeleteObject(num);//obj->code=NULL;
		return;
	};



	obj->xpos+=obj->xspeed+obj->pxspeed;
	obj->ypos+=obj->yspeed;

	if (obj->pxspeed>8192)
		obj->pxspeed-=8192;
	else
	if (obj->pxspeed<-8192)
		obj->pxspeed+=8192;
	else
		obj->pxspeed=0;

	obj->xspeed+=obj->xacc;
	if (obj->ypos>level.waterlevel)
		obj->yspeed+=obj->yacc+level.gravity/4;
	else
		obj->yspeed+=obj->yacc+level.gravity;

	if (obj->counter>obj->lifetime)
	{
		obj->state=sEXPLODE;
		obj->phase=0;
		obj->counter=0;
		obj->lighttype=0;
	}
/*
	//destroy bullets when out of range
	if (obj->creator>=PLAYSLOT)
	{
		parent=(Tgameobj *)&player[obj->creator-PLAYSLOT];
		pdx=ABS(obj->xpos-parent->xpos)/65536;
		pdy=ABS(obj->ypos-parent->ypos)/65536;

		if ((pdx>400) || (pdy>200))
		{
			obj->state=sEXPLODE;
			obj->phase=0;
			obj->counter=0;
			obj->lighttype=0;
		};
	} else
	{
		parent=&gameobj[obj->creator];
		pdx=ABS(obj->xpos-parent->xpos)/65536;
		pdy=ABS(obj->ypos-parent->ypos)/65536;
		if ((parent->state==sKILL) || (pdx>320) || (pdy>200))
		{
			obj->state=sEXPLODE;
			obj->phase=0;
			obj->counter=0;
			obj->lighttype=0;
			return;
		};
	};
*/

	if (obj->xspeed>6*65536) obj->xspeed=6*65536;
	else
	if (obj->xspeed<-6*65536) obj->xspeed=-6*65536;

	if (obj->yspeed>6*65536) obj->yspeed=6*65536;
	else
	if (obj->yspeed<-6*65536) obj->yspeed=-6*65536;


	goup=CheckPixel(obj->xpos,obj->ypos-4*65536);
	godown=CheckPixel(obj->xpos,obj->ypos+4*65536);
	goleft=CheckPixel(obj->xpos-2*65536,obj->ypos);
	goright=CheckPixel(obj->xpos+2*65536,obj->ypos);

	if ((obj->yspeed<0) && (goup==1))
	{
		obj->var1++;
		obj->yspeed=-(obj->yspeed*(9+(int)RandFac(7)))/8;
		sample++;
	} else
	if ((obj->yspeed>0) && (godown==1))
	{
		obj->var1++;
		obj->yspeed=-(obj->yspeed*9)/8;
		sample++;
	};

	if ((obj->xspeed<0) && (goleft==1))
	{
		obj->var1++;
		obj->xspeed=-(obj->xspeed*9)/8;
		if (!RandFac(3)) obj->xacc=-obj->xacc;
		sample++;
	} else
	if ((obj->xspeed>0) && (goright==1))
	{
		obj->var1++;
		obj->xspeed=-(obj->xspeed*9)/8;
		if (!RandFac(3)) obj->xacc=-obj->xacc;
		sample++;
	};

	if (sample)
		PlaySample(obj->xpos,obj->ypos,sCOMMON_BLOKPLOP,0,0);


	if (obj->var1>32) //bounces
	{
		obj->state=sEXPLODE;
		obj->phase=0;
		obj->counter=-1;
		obj->lighttype=0;
	};

	
	obj->counter++;

	if ((obj->counter&3)==0)
	{
		obj->phase++;
		if (obj->phase>=anims[obj->curanim].numframes)
			obj->phase=0;

		obj->curframe=anims[obj->curanim].framelist[obj->phase];
	}

	
	if (obj->counter>6) //DONT DISPLAY INSIDE CHAR!
		DrawGameobj(num); //tho never freezed

} //cBOUNCE2



void cBUBBLYSPRAY(int num)
{
	Obullets *obj;
//	int radius;
	int goup,godown,goleft,goright;//,sample;
	int dx,dy,dr;


	obj=(Obullets *)(&gameobj[num]);

	if (obj->state==sSTART)
	{
		ASSERTT(obj->creator >= PLAYSLOT, "unsupported creator of bubblyspray");

//		obj->xpos+=obj->direction*8*65536;
		obj->yspeed=sintable(gamecounter*32+Rand2Fac(63));
		obj->state=sFLY;	
	}
	else
	if (obj->state==sFLY)
	{
		obj->xpos+=obj->xspeed+obj->pxspeed;
		obj->ypos+=obj->yspeed;

		if (obj->pxspeed>8192)
			obj->pxspeed-=8192;
		else
		if (obj->pxspeed<-8192)
			obj->pxspeed+=8192;
		else
			obj->pxspeed=0;

		obj->xspeed+=obj->xacc;
		obj->yspeed-=4096/2;//obj->yacc;


		if (obj->counter>20)
		{
			dx=(player[obj->creator-PLAYSLOT].xpos-obj->xpos)/65536;
			dy=(player[obj->creator-PLAYSLOT].ypos-obj->ypos)/65536;

			dr=dx*dx+dy*dy;

			if (dr<32*32)
			{
				obj->xspeed=-obj->xspeed/2;
				obj->xpos+=obj->xspeed; //extra decrease
				obj->yspeed=-obj->yspeed;
				obj->xacc=-obj->xacc;
			};
		};

//		sample=0; //no sample for bouncing
		goup=CheckPixel(obj->xpos,obj->ypos-4*65536);
		godown=CheckPixel(obj->xpos,obj->ypos+4*65536);
		goleft=CheckPixel(obj->xpos-4*65536,obj->ypos);
		goright=CheckPixel(obj->xpos+4*65536,obj->ypos);

		if ((obj->yspeed<0) && (goup==1))
		{
			obj->yspeed=-obj->yspeed;
//				sample++;
		} else
		if ((obj->yspeed>0) && (godown==1))
		{
			obj->yspeed=-obj->yspeed;
//				sample++;
		};

		if ((obj->xspeed<0) && (goleft==1))
		{
			obj->xspeed=-obj->xspeed/2;
			obj->xacc=-obj->xacc;
//				sample++;
		} else
		if ((obj->xspeed>0) && (goright==1))
		{
			obj->xspeed=-obj->xspeed/2;
			obj->xacc=-obj->xacc;
//				sample++;
		};

	} else
	if (obj->state==sEXPLODE)
	{
		//dont move anymore, just play animation.
		if ((obj->counter&3)==0)
			obj->phase++;	//two times faster

	} else
	if ((obj->state==sKILL) || (obj->state==sDEACTIVATE))
	{
		MyDeleteObject(num);//obj->code=NULL;
		return;
	}


	if ((obj->counter&7)==0)
		obj->phase++;

	if (obj->phase>=anims[obj->curanim].numframes)
	{
		MyDeleteObject(num);//obj->code=NULL;
		return;
	};
	
	obj->curframe=anims[obj->curanim].framelist[obj->phase];

	obj->counter++;
	if (obj->counter>8)
		DrawGameobj(num);

} //cBUBBLYSPRAY


void rocket1(int num)
{
	Obullets *obj;
	Omonster *monster;

	obj=(Obullets *)(&gameobj[num]);

	switch (obj->state)
	{

	case sSTART:
		if (obj->creator>=PLAYSLOT)
		{	//CREATED BY PLAYER!
//			obj->direction=player[obj->creator-PLAYSLOT].direction;
//			obj->xspeed=(obj->direction*obj->xspeed)+player[obj->creator-PLAYSLOT].xspeed;
//			obj->xacc=(obj->direction*obj->xacc);
		}
		else
		{	//CREATED BY MONSTER
			monster=(Omonster *)(&gameobj[obj->creator]);
			obj->direction=monster->direction;
			obj->xspeed=(obj->direction*obj->xspeed)+monster->xspeed;
			obj->xacc=(obj->direction*obj->xacc);
		}
		obj->state=sROCKETFLY;
		break;

	case sFREEZE:	//to be sure
	case sROCKETFLY:
		cROCKETFLY(num);
		break;

	case sEXPLODE:
		if (obj->objtype!=oEXPLOSION)
		{
			obj->counter=0;
			obj->phase=0;
			DoBlast(num,obj->xpos,obj->ypos,obj->creator,50*50);			

			if ((obj->load==aPLAYERBULLETP5) && parEnableLighting && !parLowDetail)
				AddHalo(obj->xpos,obj->ypos);

			PlaySample(obj->xpos,obj->ypos,sAMMO_BOEM1,128,0);

		};	//this will be set in lightexplode

		cLIGHTEXPLODE(num);
		break;

	case sKILL:
	case sDEACTIVATE:
		//exit, without explosion
		MyDeleteObject(num);//obj->code=NULL;
		break;
	
	default:
		FAIL("rocket1(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //switch

} //rocket1()





void cFLAMEYEXPLODE(int num)
{
	Obullets *obj;

	obj=(Obullets *)(&gameobj[num]);


	if (obj->ypos>level.waterlevel)
	{
		MyDeleteObject(num);
		return;
	}


	if ((gamecounter&7)==((unsigned int)num&7))
		obj->phase++;

	if (RandFac(7)==0)
		AddParticlePixel(obj->xpos,obj->ypos-65536*4,3);


	if (obj->phase>=anims[obj->curanim].numframes)
	{
        obj->phase=0;
        obj->counter--;
        if (obj->counter<=0)
			MyDeleteObject(num);//obj->code=NULL;
	}

	obj->curframe=anims[obj->curanim].framelist[obj->phase];
	DrawGameobj(num);

} //cFLAMEYEXPLODE()



void cFLAMEY(int num)
{
	Obullets *obj;
	int hit;

	obj=(Obullets *)(&gameobj[num]);

	if (obj->ypos>level.waterlevel)
	{
		obj->curanim=AnimBase[mCOMMON]+mCOMMON_BUBBLES;
		obj->phase=((int)RandFac(255))%3;
		obj->ProcPtr=cBUBBLE;
		obj->nohit=1;
		obj->lighttype=0;
		obj->objtype=oSPECIAL; //nonbullet
		return;
	};


	if (obj->direction)
	{
		if (obj->xspeed>0)
		{
			if (obj->xspeed>6*65536)
				obj->xspeed=6*65536;
			else
			if (obj->xspeed>1024)
				obj->xspeed-=1024;
			else
				obj->xspeed=0;
		}
		else
		{
			if (obj->xspeed<-6*65536)
				obj->xspeed=-6*65536;
			else
			if (obj->xspeed<-1024)
				obj->xspeed+=1024;
			else
				obj->xspeed=0;
		}	//obj->xacc
		obj->xpos+=obj->xspeed+obj->pxspeed;

		obj->yspeed+=obj->yacc;
		obj->ypos+=obj->yspeed;
	}
	else
	{
		if (obj->yspeed>0)
		{
			if (obj->yspeed>1024)
				obj->yspeed-=1024;
			else
				obj->yspeed=0;
		} else
		{
			if (obj->yspeed<-1024)
				obj->yspeed+=1024;
			else
				obj->yspeed=0;
		}; //obj->yacc
		obj->ypos+=obj->yspeed;

		obj->xspeed+=obj->xacc;
		obj->xpos+=obj->xspeed;
	};


	hit=CheckPixel(obj->xpos,obj->ypos);
	if (hit)
	{
        if (obj->counter==0)
        	obj->counter=3;
		obj->xspeed=0; obj->xacc=0;
		obj->yspeed=0; obj->yacc=0;
		//obj->objtype=oSPECIAL;	//not a bullet anymore: nocollision 
	
		if (hit==-2)
			obj->state=sKILL;
		else
		{
			obj->state=sEXPLODE;
			obj->counter=4;
		};

	} else
	{
		if ((obj->counter&7)==0)
			obj->phase++;

		if (obj->phase>=anims[obj->curanim].numframes)
		{
			MyDeleteObject(num);//obj->code=NULL;
			return;
		}; 
	};

	obj->curframe=anims[obj->curanim].framelist[obj->phase];

	obj->counter++;

	if (obj->counter>8)
	{
		DrawGameobj(num);
		obj->lighttype=1;
	}
	else
	if (obj->creator>=PLAYSLOT)
		obj->ypos=player[obj->creator-PLAYSLOT].ypos-obj->var1;

} //cFLAMEY()



void flamebullet(int num)
{
	Obullets *obj;
	Omonster *monster;

	obj=(Obullets *)(&gameobj[num]);

	if (obj->ypos>level.waterlevel)
	{
		obj->curanim=AnimBase[mCOMMON]+mCOMMON_SMBUBBLE;
		obj->ProcPtr=cBUBBLE;
		obj->nohit=1;
		obj->xpos+=obj->direction*8*65536;
		obj->ypos-=4*65536;
		obj->xspeed/=2;
		obj->objtype=oSPECIAL; //nonbullet
		obj->light=0;
		obj->lighttype=0;

		return;
	}	//underwater
	
	switch (obj->state)
	{

	case sSTART:
		if (obj->creator>=PLAYSLOT)
		{	//CREATED BY PLAYER!
			obj->var1=player[obj->creator-PLAYSLOT].ypos-obj->ypos;
		}
		else
		{	//CREATED BY MONSTER
			obj->var1=-4*65536;
			monster=(Omonster *)(&gameobj[obj->creator]);
			obj->direction=monster->direction;
			obj->xspeed=(obj->direction*obj->xspeed)+monster->xspeed;
			obj->xacc=(obj->direction*obj->xacc);
		};

		if (obj->direction==0)
			obj->xacc=Rand2Fac(3)*1024;
		else
			obj->yacc=Rand2Fac(3)*1024;
		obj->state=sFLY;
		break;

	case sFREEZE: //to be sure
	case sFLY:
		cFLAMEY(num);
		break;

	case sEXPLODE:
		if (obj->load==aPLAYERBULLETP6)
		{
			obj->curanim=AnimBase[mAMMO]+mAMMO_BLUEFIRE;
			if (obj->counter>2)
				obj->counter=2;

			cFLAMEYEXPLODE(num);
		}
		else
			MyDeleteObject(num);
		break;

	case sKILL:
	case sDEACTIVATE:
		MyDeleteObject(num);
		break;
	
	default:
		FAIL("flamebullet(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	} //switch

} //flamebullet()



void bomb(int num)
{
	Obullets *obj;
	int hit,px,py;


	obj=(Obullets *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		obj->state=sSLEEP;
		obj->counter=0;
		break;


	case sSLEEP:
		obj->phase=(Word8)((gamecounter/4)%(anims[obj->curanim].numframes));

		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		DrawGameobj(num);

		obj->counter++;
		if (obj->counter>obj->lifetime)
			obj->state=sEXPLODE;
		break;

	case sEXPLODE:
		if (obj->objtype!=oEXPLOSION)
		{
			obj->counter=0;
			obj->phase=0;
			DoBlast(num,obj->xpos,obj->ypos,obj->creator,100*100);
			PlaySample(obj->xpos,obj->ypos,sCOMMON_EXPL_TNT,128,0);
		};
		obj->freeze=0;

		cLIGHTEXPLODE(num);
		break;

	case sFLY:

//		obj->xspeed=(obj->xspeed*63)/64;

		obj->xpos+=obj->xspeed;
		obj->ypos+=obj->yspeed;

	//CHECK FOR FALLING
		if (obj->xspeed<0)
			px=-frames[obj->curframe].tx2-(frames[obj->curframe].dx/2);
		else
			px=(frames[obj->curframe].dx/2)+frames[obj->curframe].tx2;

		py=frames[obj->curframe].ty2+frames[obj->curframe].dy;

		hit=CheckPixel(obj->xpos+(px*65536),obj->ypos+(py*65536));

		if (hit==0)
		{
			if (obj->ypos>level.waterlevel)
				obj->yspeed+=level.gravity/4;
			else
				obj->yspeed+=level.gravity;
		}
		else
		{
			obj->yacc=0;
			obj->yspeed=0;
			obj->state=sSLEEP;
			obj->curanim=AnimBase[mAMMO]+mAMMO_BOMB;
		};


	//CHECKING FOR BOUNCING OF WALLS OR RAVINES

		if (obj->xspeed<0)
			px=-frames[obj->curframe].tx2-frames[obj->curframe].dx;
		else
			px=frames[obj->curframe].dx+frames[obj->curframe].tx2;

		py=frames[obj->curframe].ty2+frames[obj->curframe].dy-16;

		hit=CheckPixel(obj->xpos+(px*65536),obj->ypos+(py<<16));

		if (hit)
		{
			obj->xspeed=-obj->xspeed/2;
			//PlaySample();
		};


		obj->phase=(Word8)((gamecounter/4)%(anims[obj->curanim].numframes));

		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		DrawGameobj(num);

		obj->counter++;
		if (obj->counter>obj->lifetime)
		{
			obj->state=sEXPLODE;
			obj->phase=0;
			PlaySample(obj->xpos,obj->ypos,sAMMO_BOEM1,128,0);
			DoBlast(num,obj->xpos,obj->ypos,obj->creator,100*100);
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
		FAIL("bomb(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //switch

} //bomb()



void cBANANA(int num)
{
	Obullets *obj;
	int hit;
//	Tgameobj *parent;
	Omonster *monster;
//	int pdx,pdy;


	obj=(Obullets *)(&gameobj[num]);

	if (obj->state==sSTART)
	{
		ASSERT(obj->creator<LEVELSLOT);
		monster=(Omonster *)(&gameobj[obj->creator]);
		obj->direction=monster->direction;
		obj->xspeed=(obj->direction*obj->xspeed)+monster->xspeed;
		obj->xacc=(obj->direction*obj->xacc);

		obj->state=sFLY;
		obj->lifetime=AISPEED/2;
	}
	else
	if (obj->state==sEXPLODE)
	{
		obj->freeze=0;
		cEXPLODE(num);
		return;
	}
	else
	if ((obj->state==sKILL) || (obj->state==sDEACTIVATE))
	{
		MyDeleteObject(num);//obj->code=NULL;
		return;
	};

	obj->xpos+=obj->xspeed;
	obj->ypos+=obj->yspeed;

	obj->channel=PlayLoopSample(obj->xpos,obj->ypos,sMONKEY_THROW,0,0,obj->channel);

	obj->counter++;
	if (obj->counter>obj->lifetime)
	{
		obj->state=sEXPLODE;
		obj->killanim=AnimBase[mMONKEY]+mMONKEY_BANASPLT;
		obj->phase=0;
		obj->counter=0;
		obj->lighttype=0;
	}
/*
	parent=&gameobj[obj->creator];
	pdx=ABS(obj->xpos-parent->xpos);
	pdy=ABS(obj->ypos-parent->ypos);
	if ((parent->state==sKILL) || (pdx>300*65536) || (pdy>250*65536))
	{
		obj->state=sEXPLODE;
		obj->curanim=obj->killanim;
		obj->phase=0;
	};
*/
//	obj->xspeed+=obj->xacc;
	if (obj->xspeed>6*65536)
		obj->xspeed=6*65536;
	else
	if (obj->xspeed<-6*65536)
		obj->xspeed=-6*65536;

	if (obj->ypos>level.waterlevel)
		obj->yspeed+=level.gravity/4;
	else
		obj->yspeed+=level.gravity;

	if (obj->yspeed>6*65536)
		obj->yspeed=6*65536;
	else
	if (obj->yspeed<-6*65536)
		obj->yspeed=-6*65536;


	hit=CheckPixel(obj->xpos,obj->ypos);
	if (hit==1)
	{
		//banana splash
		//PlaySample(obj->xpos,obj->ypos,sCOMMON_EXPSM1,128,0);
		obj->state=sEXPLODE;
		obj->curanim=obj->killanim;
		obj->phase=0;
		PlaySample(obj->xpos,obj->ypos,sMONKEY_SPLUT,0,0);
	};

	if (hit==-2)
		MyDeleteObject(num);//obj->code=NULL;

	obj->phase=(Word8)((obj->counter/4)%(anims[obj->curanim].numframes));

	obj->curframe=anims[obj->curanim].framelist[obj->phase];

	DrawGameobj(num);

}	//cBANANA





void cTNT(int num)
{
	Obullets *obj;
	int n,dx,dy;


	obj=(Obullets *)(&gameobj[num]);

	ASSERT(obj->creator>=PLAYSLOT);

	//if ((obj->state==sSTART) || (obj->state==sDEACTIVATE))
	if (obj->state!=sEXPLODE)
		obj->state=sSLEEP;

	if (obj->state==sSLEEP)
	{
		obj->counter++;
		if (obj->counter>obj->lifetime)
		{
			obj->counter=0;
			obj->state=sEXPLODE;
		}
		else
		if ((obj->counter&3)==0)
		{

		//show counting animation
			obj->phase++;
			if (obj->phase>=anims[obj->curanim].numframes)
				obj->phase=0;

		//check for near players
			if ((MultiPlayerType==gameBATTLE) ||
				(MultiPlayerType==gameCAPTURE) ||
				(MultiPlayerType==gameTREASURE) ||
				(MultiPlayerType==gameRACE))
			{
				for (n=0;n<MAXPLAYERS;n++)
				if ((obj->creator-PLAYSLOT!=n) && player[n].Active && (player[n].flicker==0))
				{
					dx=(obj->xpos-player[n].xpos)/65536;
					if ((dx>-32) && (dx<32))
					{
						dy=(obj->ypos-player[n].ypos)/65536;
						if ((dy>-32) && (dy<32))
						{
							obj->counter=0;
							obj->state=sEXPLODE;
							break;	//nomore
						}
					}
				}	//for each active player
			}


		//check for near objects!
			if (obj->state!=sEXPLODE)
			for (n=1;n<NumObjects;n++)
			if (//(num!=n) &&	//!oTRIGGERTNT, not needed
				gameobj[n].ProcPtr && 
				((gameobj[n].objtype&oTRIGGERTNT)==oTRIGGERTNT))
			{
				dx=(obj->xpos-gameobj[n].xpos)/65536;
				if ((dx>-64) && (dx<64))
				{
					dy=(obj->ypos-gameobj[n].ypos)/65536;
					if ((dy>-64) && (dy<64))
					{
						obj->counter=0;
						obj->state=sEXPLODE;
						break;	//nomore
					}	//dy
				}	//dx
			}	//for each active gameobj

		}	//if objcounter&3==0

		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
	}
	else
	if (obj->state==sEXPLODE)
	{
		obj->counter++;

		if ((obj->counter&3)==3)
		{	
			obj->phase++;
			if (obj->phase>=anims[obj->curanim].numframes)
				obj->phase=0;
		}	//show counting down animation

	
		if (obj->counter<20)
		{
			if (obj->counter==1)
			{
				if (RandFac(1))
					obj->var1=sCOMMON_BELL_FIRE;
				else
					obj->var1=sCOMMON_BELL_FIRE2;
			}	//counter=1

			obj->channel=PlayLoopSample(obj->xpos,obj->ypos,obj->var1,0,0,obj->channel);

			obj->curframe=anims[obj->curanim].framelist[0];
			n=(256+(obj->counter-50)*(obj->counter-50))/4;
			AddScaledSprite(obj->xpos,obj->ypos,SPRITELAYER-1,n,obj->curframe);
		}
		else
		{	//do light and explosion
			if (obj->objtype!=oEXPLOSION)
			{
				PlaySample(obj->xpos,obj->ypos,sCOMMON_EXPL_TNT,128,0);

				AddExplosion(obj->xpos,obj->ypos,obj->killanim);

				obj->nohit=16;
				DoFullBlast(num,obj->xpos,obj->ypos,obj->creator,96*96);
				

				obj->objtype=oEXPLOSION;
				obj->lighttype=5;
				obj->light=16+2;
			}

			obj->light--;
			if (obj->light<2) 
			{
				obj->lighttype=0;
				obj->light=0; //ofzo
				MyDeleteObject(num);//obj->code=NULL;
			}	//light<2
		}	//light
	}	//state=sEXPLODE!

} //cTNT()





static int TraceLine(int x0,int y0,int x1,int y1)
{
	int dx,dy,t;


	dx=(x1-x0)/64;
	dy=(y1-y0)/64;
	t=0;

	do {
		t++;

		if (CheckFullPixel(x0,y0))
			return t;

		x0+=dx;
		y0+=dy;
	} while (t<64);

	return 0;
} //TraceLine()




void cSEEKINGROCKET(int num)
{
	Obullets *obj;
	int		dx,dy;
	int		dx2,dy2;
	int		retarget;
	int		numo;
	int		nump;
	int		mindist,distance;



	obj=(Obullets *)(&gameobj[num]);

	switch (obj->state)
	{

	case sSTART:
		ASSERT(obj->creator>=PLAYSLOT);
		obj->var1=0; //no target yet!

		obj->xspeed+=obj->pxspeed; obj->pxspeed=0;
		obj->yspeed+=obj->pyspeed; obj->pyspeed=0;
		//break;

	case sFREEZE:
		obj->state=sROCKETFLY;
		//break;

	case sROCKETFLY:


		if ((MultiPlayerType==gameTREASURE) ||
			(MultiPlayerType==gameCAPTURE) ||
			(MultiPlayerType==gameBATTLE))
		{

			if (obj->var1==0)	//seek target only once!
			{
				mindist=256*256;
				for (nump=0;nump<MAXPLAYERS;nump++)
				if ((nump!=obj->creator-PLAYSLOT) &&
					(player[nump].flicker==0) &&
					(player[nump].energy>0))
				{

					if ((MultiPlayerType==gameCAPTURE) &&
						(player[nump].Team==player[obj->creator-PLAYSLOT].Team))
						continue;

					dx=obj->xpos-player[nump].xpos;

					dx/=65536;
					distance=dx*dx;

					if (distance<mindist)
					{
						dy=(obj->ypos-player[nump].ypos)/65536;

						distance+=dy*dy;
						if (distance<mindist)
						{
							if (TraceLine(obj->xpos,obj->ypos,player[nump].xpos,player[nump].ypos)==0)
							{
								obj->var1=PLAYSLOT+nump;
								obj->var2=0;
								obj->var3=0;
								mindist=distance;
							}
						}	//distancey
					}	//if distancex<mindist
				} //for nump

				if (obj->var1 && (MultiPlayerConnection>gameLOCAL))
				{
					Net_SendTarget(num,obj->var1);
				}
			}	//target locked


			if ((obj->counter&7)==0)
			{
				if ((obj->var1>=PLAYSLOT) && (obj->var1<PLAYSLOT+MAXPLAYERS) && (TraceLine(obj->xpos,obj->ypos,player[obj->var1-PLAYSLOT].xpos,player[obj->var1-PLAYSLOT].ypos)==0))
				{
					obj->var3=1;
				}
				else
					obj->var3=0;
			}

			//if var3 is set: player is visible, and

		}
		else
		{
			retarget=0;

			if (obj->var1==0) 
				retarget=1;

			if ((obj->var1<PLAYSLOT) &&
				((gameobj[obj->var1].energy>0) ||
				((int)gameobj[obj->var1].ProcPtr!=obj->var2)))
				retarget=1;

			if (retarget)
			{
				obj->var1=0; //go straight ahed
				obj->var3=0;


				mindist=256*256;
				for (numo=1;numo<NumObjects;numo++)
				if ((gameobj[numo].ProcPtr) && 
					((gameobj[numo].objtype&oENEMY)==oENEMY))
				{
					dx=obj->xpos-gameobj[numo].xpos;
					dx/=65536;

					distance=dx*dx;

					if (distance<mindist)
					{
						dy=(obj->ypos-gameobj[numo].ypos)/65536;

						distance+=dy*dy;
						if (distance<mindist)
						{
							if (TraceLine(obj->xpos,obj->ypos,gameobj[numo].xpos,gameobj[numo].ypos)==0)
							{
								obj->var1=numo;
								obj->var2=(int)gameobj[numo].ProcPtr;
								obj->var3=1;	//go...
								mindist=distance;
							}
						}	//distancey
					}	//if distancex<mindist
				} //for numo
			}	//retarget
		}	//normal game

		if (obj->var1 && obj->var3)
		{
			if (obj->var1>=PLAYSLOT)
			{
				dx=player[obj->var1-PLAYSLOT].xpos-obj->xpos;
				dy=player[obj->var1-PLAYSLOT].ypos-obj->ypos;
			}
			else
			{
				dx=gameobj[obj->var1].xpos-obj->xpos;
				dy=gameobj[obj->var1].ypos-obj->ypos;
			}

			dx2=dx/(65536*4);
			dy2=dy/(65536*4);
			
			distance=(dx2*dx2+dy2*dy2);
			if (distance<2048)
				distance=SQRT[distance];
			else
				distance=64;	//whatever

			if (distance>1)
			{
				distance=(3*256)/distance;

				dx=(dx*distance)/128;
				dy=(dy*distance)/128;
			}

			obj->xspeed=(dx+obj->xspeed)/8;
			obj->yspeed=(dy+obj->yspeed)/8;

			if (obj->xspeed>0)
				obj->direction=1;
			else
			if (obj->xspeed<0)
				obj->direction=-1;
		}

		
		obj->xpos+=obj->xspeed;
		obj->ypos+=obj->yspeed;

		if (CheckFullPixel(obj->xpos,obj->ypos))
		{
			obj->state=sEXPLODE;

			cLIGHTEXPLODE(num);
			return;
		}


		dx=ABS(obj->xspeed);
		dy=ABS(obj->yspeed);

		obj->counter++;
		if (obj->counter>obj->lifetime)
		{
			obj->state=sEXPLODE;
			cLIGHTEXPLODE(num);
			return;
		}

		if ((obj->counter&7)==0)
		{
			if (obj->type==bullPOWER)
			{
				if (dx<32768) //up/down
				{
					if (obj->yspeed<0)
						obj->curanim=AnimBase[mAMMO]+mAMMO_GUN4PU;
					else
						obj->curanim=AnimBase[mAMMO]+mAMMO_GUN4PD;
				}
				else
				{
					if (dy<32768)
						obj->curanim=AnimBase[mAMMO]+mAMMO_GUN4PR;
					else
					if (obj->yspeed<0)
						obj->curanim=AnimBase[mAMMO]+mAMMO_GUN4PUR;
					else
						obj->curanim=AnimBase[mAMMO]+mAMMO_GUN4PDR;
				}
			}
			else
			{
				if (dx<32768) //up/down
				{
					if (obj->yspeed<0)
						obj->curanim=AnimBase[mAMMO]+mAMMO_GUN4NU;
					else
						obj->curanim=AnimBase[mAMMO]+mAMMO_GUN4ND;
				}
				else
				{
					if (dy<32768)
						obj->curanim=AnimBase[mAMMO]+mAMMO_GUN4NR;
					else
					if (obj->yspeed<0)
						obj->curanim=AnimBase[mAMMO]+mAMMO_GUN4NUR;
					else
						obj->curanim=AnimBase[mAMMO]+mAMMO_GUN4NDR;
				}
			}
		} //counter&7



		obj->phase=(obj->counter/4);
		if (obj->phase>=anims[obj->curanim].numframes)
			obj->phase=0;

		obj->curframe=anims[obj->curanim].framelist[obj->phase];
		DrawGameobj(num);
		break;


	case sEXPLODE:
		if (obj->objtype!=oEXPLOSION)
		{
			obj->counter=0;
			obj->phase=0;
			DoBlast(num,obj->xpos,obj->ypos,obj->creator,50*50);			
		};	//this will be set in lightexplode

		cLIGHTEXPLODE(num);
		break;

	case sKILL:
	case sDEACTIVATE:
		//exit, without explosion
		MyDeleteObject(num);//obj->code=NULL;
		break;
	
	default:
		FAIL("cSEEKINGROCKET(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //switch
} //cSEEKINGROCKET()




void cSPREAD(int num)
{
	Obullets *obj;


	obj=(Obullets *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		ASSERT(obj->creator>=PLAYSLOT);
		obj->var1=0; //no target yet!
		obj->state=sFLY;
		obj->phase=0;
		obj->curanim=AnimBase[mAMMO]+mAMMO_CASELESS;
		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		//break;


	case sFLY:
		obj->counter++;

		obj->xspeed+=obj->xacc;
		obj->xpos+=obj->xspeed+obj->pxspeed;

		if (obj->pxspeed>8192)
			obj->pxspeed-=8192;
		else
		if (obj->pxspeed<-8192)
			obj->pxspeed+=8192;
		else
			obj->pxspeed=0;

		obj->yspeed+=obj->yacc;
		obj->ypos+=obj->yspeed;


		if ((obj->counter>54) || CheckPixel(obj->xpos,obj->ypos))
		{
			obj->state=sEXPLODE;
			obj->curanim=AnimBase[mAMMO]+mAMMO_CASEPLOF;
			obj->curframe=anims[obj->curanim].framelist[0];
			obj->counter=0;
			obj->light=2;
			obj->lighttype=1;
		}

		DrawGameobj(num);
		break;


	case sEXPLODE:

		obj->counter++;

		obj->curframe=anims[obj->curanim].framelist[obj->counter/4];

		if (obj->counter>4*4-1)
			MyDeleteObject(num);
		else
			DrawGameobj(num);
		break;

	case sKILL:
	case sDEACTIVATE:
		//exit, without explosion
		MyDeleteObject(num);//obj->code=NULL;
		break;
	
	default:
		FAIL("cSPREAD(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //switch

}	//cSPREAD()






void cBLAMMER(int num)
{
	Obullets *obj;
	int x,y,frame;
	int a,n,radius;
	int sx,sy;


	obj=(Obullets *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		ASSERT(obj->creator>=PLAYSLOT);

		obj->var1=0; //no target yet!
		obj->state=sFLY;
		obj->phase=0;
		//obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
		obj->curanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		obj->light=3;
		obj->lighttype=1;

		//break;

	case sFLY:
		obj->counter++;
		if (obj->counter>54)
			obj->state=sEXPLODE;

		obj->xspeed+=obj->xacc;
		obj->xpos+=obj->xspeed+obj->pxspeed;

		obj->yspeed+=obj->yacc;
		obj->ypos+=obj->yspeed+obj->pyspeed;


		if (obj->direction>0)
			obj->var1+=32;
		else
			obj->var1-=32;


		a=num*128+obj->var1;
		n=8;
		frame=obj->curframe;
		radius=64+obj->counter*8;

		do {
			x=obj->xpos+(radius*costable(a))/64;
			y=obj->ypos+(radius*sintable(a))/64;

			AddSprite(x,y,SPRITELAYER,0,0,0,frame);

			a+=32*3;

			if (!RandFac(3))
			{
				sx=x-obj->xpos;
				sy=y-obj->ypos;

				AddParticleSparkles(x,y,sx/4,sy/4);
			}

		} while (--n>0);

		break;


	
	case sEXPLODE:
	case sKILL:
	case sDEACTIVATE:
		//exit, without explosion
		MyDeleteObject(num);//obj->code=NULL;
		break;
	
	default:
		FAIL("cBLAMMER(): Illegal state: obj:%i, type:%i, state:%i",num,obj->load,obj->state);
		break;
	}; //switch

}	//cBLAMMER()


