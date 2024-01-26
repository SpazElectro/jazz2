/*
	G_AIBIRD.C


  ai routines for birdy!


*/
#include "G_Ai.h"
#include "Random.h"
#include "E_Loaders.h"
#include "E_Sound.h"
#include "E_Graphics.h"
#include "G_Event.h"
#include "Globals.h"
#include "G_Objects.h"
#include "G_Collision.h"
#include "E_Parallax.h"
#include "G_Hit.h"
#include "G_Bullet.h"
#include "G_AiTypes.h"

#if _DEBUG
#define FAIL	Fatal
#else
#define FAIL	1 ? (void)0 : Fatal
#endif







static void birdcircle(int num)
{
	Omonster *obj;
	Tplayer *play;
	int oldpos;

	obj=(Omonster *)(&gameobj[num]);
	play=&player[obj->var1-PLAYSLOT];


	if (play->xpos!=obj->var2)
	{
		obj->state=sFLY;
		obj->curanim=AnimBase[obj->loadanim]+mBIRD_FLYNORM;
	}

	oldpos=obj->xpos;
	obj->xpos=obj->var2+96*sintable(obj->var3);
	obj->var3+=8;

	if (obj->xpos<oldpos) 
		obj->direction=-1;
	else
		obj->direction=1;

	if (obj->ypos>level.waterlevel-12*65536) obj->ypos=level.waterlevel-12*65536;

	if ((gamecounter&7)==0)
	{
		obj->phase++;
		if (obj->phase>=anims[obj->curanim].numframes)
			obj->phase=0;

		obj->curframe=anims[obj->curanim].framelist[obj->phase];
	}
	DrawGameobj(num);
} //birdcircle()



static void birdidle(int num)
{
	Omonster *obj;
	Tplayer *play;
	int tx,ty;
	int sx,sy;
	int px,py;
	int tileup,tiledown;
	int fx,fy;
	int x,y;
	int tempx,tempy;


	obj=(Omonster *)(&gameobj[num]);
	play=&player[obj->var1-PLAYSLOT];


	if (play->idletime==0)
	{
		obj->curanim=AnimBase[obj->loadanim]+mBIRD_FLYNORM;
		obj->state=sFLY;
		obj->var5=0;
	}
	else
	{


    	if (obj->var3==0)
    	{
	        sx=(obj->xpos/(65536*32))-4; //blocks!
			sy=(obj->ypos/(65536*32))-3;

			fx=fy=0;


			px=play->xpos/65536;
			py=play->ypos/65536;
			
			for (y=0;y<8;y++)
			{
				ty=sy+y;
				if ((Word)ty>(Word)(level.blockheight-2))
					continue;

				for (x=0;x<6;x++)
				{
					tx=sx+x;
					if ((Word)tx>level.blockwidth)
						continue;

					if ((ABS(tx-px)+ABS(ty-py))<3) //too close to player!
						continue;

					tileup=GetSpriteLayerTile(tx, ty);

					if (TileSet.Mask[tileup]==TileSet.Mask[0]) //zero mask!
					{
						tiledown=GetSpriteLayerTile(tx, ty+1);

						if (tiledown && (TileSet.Mask[tiledown]!=TileSet.Mask[0]))
						{
							fx=tx;
							fy=ty;
							break;
						}

					}
				}
			}

			if ((fx==0) && (fy==0)) //not found!
			{
				obj->var5=0; //birdidle
				obj->state=sFLY;
				obj->curanim=AnimBase[obj->loadanim]+mBIRD_FLYNORM;
				obj->counter=0;
				DrawGameobj(num); //draw once
				return;
			}

            obj->creator=0;
            obj->xorg=(fx*32+15)*65536;
            obj->yorg=(fy*32+2)*65536;


			tempx=obj->xpos;
			tempy=obj->ypos;
			obj->xpos=obj->xorg;
			obj->ypos=obj->yorg;
			obj->curanim=AnimBase[obj->loadanim]+mBIRD_STILL;
			obj->curframe=anims[obj->curanim].framelist[0];
			PutObjectOnGround(num); //get landing position

			obj->phase=0;
			obj->var3=3;

			obj->yorg=obj->ypos;
			obj->xpos=tempx;
			obj->ypos=tempy;
			
			obj->xspeed=(obj->xorg-obj->xpos)/32;
			obj->yspeed=(obj->yorg-obj->ypos)/32;


            if (obj->xspeed<0)
                obj->direction=-1;
            else
                obj->direction=+1;

			obj->counter=0;
    		obj->var3=1; //flying
			obj->phase=0;
			obj->curanim=AnimBase[obj->loadanim]+mBIRD_FLYNORM;
		}
		else
		if (obj->var3==1)
		{
			obj->xpos+=obj->xspeed;
			obj->ypos+=obj->yspeed;

			if (obj->counter>31)
			{
				obj->direction=play->direction;
				obj->var3=3; //still
				obj->curanim=AnimBase[obj->loadanim]+mBIRD_STILL;
				obj->phase=0;
			}

			if ((obj->phase==6) && ((obj->counter&7)==7))
			{
				if (RandFac(1))
					PlaySample(obj->xpos,obj->ypos,sCOMMON_BIRDFLY,40,0);
				else
					PlaySample(obj->xpos,obj->ypos,sCOMMON_BIRDFLY2,40,0);
			}
		}	//var3==1


		obj->counter++;
		if ((obj->counter&7)==0)
			obj->phase++;

		if (obj->phase>=anims[obj->curanim].numframes)
		{
			obj->phase=0;


			switch (obj->var3)
			{
			case 2: //worm
				obj->var3=3; //still anim
				obj->curanim=AnimBase[obj->loadanim]+mBIRD_STILL;
				break;

			case 3:
				if ((CheckPixel(obj->xpos,obj->ypos+20*65536)) && (RandFac(7)==4))
				{
					obj->var3=2; //WORM!
					obj->curanim=AnimBase[obj->loadanim]+mBIRD_IDLE;
				}
				else
				if (RandFac(3)==0)	//random (otherwise stay in this idle anim)
				{
					if (RandFac(1))
					{
						obj->var3=4; //look left!
						obj->curanim=AnimBase[obj->loadanim]+mBIRD_LOOKL1;
					}
					else
					{
						obj->var3=7; //look right!
						obj->curanim=AnimBase[obj->loadanim]+mBIRD_LOOKR1;
					}
				} 
				break;

			case 4:
				obj->var3=5;
				obj->curanim=AnimBase[obj->loadanim]+mBIRD_LOOKL2;
				break;

			case 5:
				if (RandFac(3)==0)
				{
					obj->var3=6;
					obj->curanim=AnimBase[obj->loadanim]+mBIRD_LOOKL3;
				}
				break;

			case 6:
				obj->var3=3;			//return to still animation
				obj->curanim=AnimBase[obj->loadanim]+mBIRD_STILL;
				break;

			case 7:
				obj->var3=8;			//go looking RIGHT
				obj->curanim=AnimBase[obj->loadanim]+mBIRD_LOOKR2;
				break;

			case 8:
				if (RandFac(3)==0)
				{
					obj->var3=9;		//keep looking randomly
					obj->curanim=AnimBase[obj->loadanim]+mBIRD_LOOKR3;
				}
				break;

			case 9:
				obj->var3=3;			//return to still animation
				obj->curanim=AnimBase[obj->loadanim]+mBIRD_STILL;
				break;

			default:
				break;
			} //switch

		} //if phase>numframes
		
	} //if player still idling



	obj->curframe=anims[obj->curanim].framelist[obj->phase];
	DrawGameobj(num);

} //birdidle()

	

static void birdattack(int num)
{
	Omonster *obj;
	Tplayer *play;
	int numhits;
//	int tx,cx,cy,y,miny,minx;


	obj=(Omonster *)(&gameobj[num]);
	play=&player[obj->var1-PLAYSLOT];


	obj->xpos+=obj->xspeed;
	obj->ypos+=obj->yspeed;
	if (obj->ypos>level.waterlevel-12*65536) 
		obj->ypos=level.waterlevel-12*65536;

	obj->counter++;
	if ((obj->counter>30) && (!gameobj[obj->var3].ProcPtr))
	{
		obj->state=sFLY;
		obj->curanim=AnimBase[obj->loadanim]+mBIRD_FLYNORM;
		obj->counter=0;
	}
	else
	{
		numhits=PixelCollision(&gameobj[obj->var3],&gameobj[num]);
		if (numhits>7)	//???
		{
			HitObject(obj->var3,obj->var1,-4);
			obj->state=sFLY;
			obj->curanim=AnimBase[obj->loadanim]+mBIRD_FLYNORM;
			obj->counter=0;
		}
	}

	if ((obj->counter&7)==0)
	{
		obj->phase++;
		if (obj->phase>=anims[obj->curanim].numframes)
			obj->phase=0;
		obj->curframe=anims[obj->curanim].framelist[obj->phase];
	}

	DrawGameobj(num);

} //birdattack()




static void birdfly(int num)
{
	Omonster *obj;
	Tplayer *play;
	int numo,dx,dy;
	int cdx,cdy,aaa;
	int res;
	int wanty;


	obj=(Omonster *)(&gameobj[num]);

	
	play=&player[obj->var1-PLAYSLOT];


	obj->xspeed=(play->xpos-(play->direction*32*65536)-obj->xpos)/32;
	
	
	wanty=play->ypos-(play->goup*55*65536);
	if (play->dive)
		wanty+=40*65536;

	obj->yspeed=(wanty-obj->ypos-(4*sintable(gamecounter*16)))/32;


	if (play->idletime>0)
		obj->var5++;
	else
		obj->var5=0;


	if (obj->var5>=70*4)
	{
		obj->state=sIDLE;
		obj->var3=0;
	}
	else
	{
		obj->direction=play->direction;
/*		if (obj->xspeed<0)
			obj->direction=-1;
		else
			obj->direction=1;
*/
		obj->xpos+=obj->xspeed;
		obj->ypos+=obj->yspeed;
		if (obj->ypos>level.waterlevel-12*65536) obj->ypos=level.waterlevel-12*65536;
	}


	obj->counter++;

	if ((obj->counter>2*AISPEED) && ((obj->counter&7)==0))
//attack
	for (numo=1;numo<NumObjects;numo++)
	if ((gameobj[numo].ProcPtr) && ((gameobj[numo].objtype&oENEMY)==oENEMY))
	{
		dx=(gameobj[numo].xpos-obj->xpos);
		dy=(gameobj[numo].ypos-obj->ypos);

		if (obj->loadanim==mCHUCK)
		{
			if ((dy>2*65536) && (dy<150*65536))
			{
				cdx=ABS(dx);
				cdy=dy/65536;

				aaa=cdx/cdy;

				if ((aaa>40000) && (aaa<80000))
				{
					obj->xspeed=dx/32;
					obj->yspeed=dy/32;

					if (obj->xspeed<0) 
						obj->direction=-1;
					else
						obj->direction=1;

					obj->counter=0;
					obj->state=sATTACK;			
					obj->curanim=AnimBase[obj->loadanim]+mBIRD_ATTKDOWN;
					obj->var3=numo;
					
					break;
				}
			}
		}
		else
		if ((dy>-16*65536) && (dy<64*65536))
		{	//normal bird: shoot!

			if (((obj->direction>0) && (dx>0)) ||
				((obj->direction<0) && (dx<0)))
			{
				res=AddBullet(num,aPLAYERBULLET1);
				if (res>0)
				{
					gameobj[res].creator=obj->var1;
				}
				res=AddBullet(num,aPLAYERBULLET1);
				if (res>0)
				{
					gameobj[res].creator=obj->var1;
					gameobj[res].yspeed=3*32768;
				}


				obj->counter=2*AISPEED-AISPEED/2; //2 bulls a second!
			}
		}

	}  //for each active object


	obj->curanim=AnimBase[obj->loadanim]+mBIRD_FLYNORM;

	if ((gamecounter&7)==0)
	{
		obj->phase++;
		if (obj->phase>=anims[obj->curanim].numframes)
			obj->phase=0;
		obj->curframe=anims[obj->curanim].framelist[obj->phase];

		if (obj->phase==6)
		{
			if (RandFac(1))
				PlaySample(obj->xpos,obj->ypos,sCOMMON_BIRDFLY,40,0);
			else
				PlaySample(obj->xpos,obj->ypos,sCOMMON_BIRDFLY2,40,0);
		}
	}
	
	DrawGameobj(num);
} //birdfly()



static void birdflyaway(int num)
{
	Omonster *obj;

	obj=(Omonster *)(&gameobj[num]);


	obj->xpos+=obj->direction*2*65536;
	obj->ypos-=65536;



	if (!InViewArea(obj->xpos,obj->ypos))
	{
		MyDeleteObject(num);//obj->code=NULL;
		return;
	};	

	obj->curanim=AnimBase[obj->loadanim]+mBIRD_FLYNORM;

	if ((gamecounter&7)==0)
		obj->phase++;
	if (obj->phase>=anims[obj->curanim].numframes)
		obj->phase=0;
	obj->curframe=anims[obj->curanim].framelist[obj->phase];


	if (((gamecounter&7)==0) && (obj->phase==6))
	{
		if (RandFac(1))
			PlaySample(obj->xpos,obj->ypos,sCOMMON_BIRDFLY,40,0);
		else
			PlaySample(obj->xpos,obj->ypos,sCOMMON_BIRDFLY2,40,0);
	};

	obj->curframe=anims[obj->curanim].framelist[obj->phase];
	DrawGameobj(num);


} //birdflyaway()



static void birdhurt(int num)
{
	Omonster *obj;
	Omonster *obj2;
	int numfeathers,cobj;

	obj=(Omonster *)(&gameobj[num]);

	if (obj->curanim!=AnimBase[obj->loadanim]+mBIRD_HURT)
	{
		obj->curanim=AnimBase[obj->loadanim]+mBIRD_HURT;
		obj->counter=0;

		numfeathers=8;
		do{
			cobj=AddObject(obj->xpos,obj->ypos,aSHARD,0);
			if (cobj>0)
			{
				obj2=(Omonster *)(&gameobj[cobj]);
				obj2->ProcPtr=&cFEATHER;
				switch (RandFac(3))
				{
					case 0:
						obj2->curanim=AnimBase[obj->loadanim]+mBIRD_FEATHR1;
						break;
					case 1:
						obj2->curanim=AnimBase[obj->loadanim]+mBIRD_FEATHR2;
						break;
					default:
						obj2->curanim=AnimBase[obj->loadanim]+mBIRD_FEATHR3;
						break;
				}; //switch
			};
			numfeathers--;
		} while (numfeathers>0);

//		PlaySample(obj->xpos,obj->ypos,sCOMMON_BIRDFLY,40,0);
	};


	obj->counter++;
	if (obj->counter>3)
	{
		obj->counter=0;
		obj->phase++;



		if (obj->phase>=anims[obj->curanim].numframes)
		{
			obj->state=sHIDE;
			obj->curanim=AnimBase[obj->loadanim]+mBIRD_FLYNORM;
			obj->phase=0;
		};
	};

	obj->curframe=anims[obj->curanim].framelist[obj->phase];
	DrawGameobj(num);

} //birdhurt()


void birdy(int num)
{
	Omonster *obj;

	obj=(Omonster *)(&gameobj[num]);


	if (obj->var1<PLAYSLOT)
	{
		MyDeleteObject(num);
		return;
	}

	if (player[obj->var1-PLAYSLOT].bird==0)
		obj->state=sHIDE;


	switch (obj->state)
	{
	case sFREEZE: //fall thru all this haha
		obj->freeze=0;

	case sSTART:
//	case sACTION: 
		obj->state=sFLY;
		obj->curanim=AnimBase[obj->loadanim]+mBIRD_FLYNORM;
		break;

	case sFLY:
		birdfly(num);
		break;

	case sIDLE:
		birdidle(num);
		break;

	case sHIT:
		birdhurt(num);
		break;
	case sHIDE:
		birdflyaway(num);
		break;

	case sATTACK:
		birdattack(num);
		break;

	case sKILL:
		cKILL(num);
		break;

	case sDEACTIVATE:
		//only can happen when still in cage!
	
		if ((obj->var1<PLAYSLOT) || (player[obj->var1-PLAYSLOT].bird==0))
		{
			MyDeleteObject(num);//obj->code=NULL;
		}
		else
		{
			obj->state=sFLY;
			birdfly(num); //get it back to player!
		};
		break;

	default:
		FAIL("birdy(): Illegal state: obj:%i, type:%i, state:%i\n",num,obj->load,obj->state);
		break;
	}; //switch

} //birdy()



void birdcage(int num)
{
	Omonster *obj;
	Omonster *obj2;
//	Tplayer *play;
	int cobj;


	obj=(Omonster *)(&gameobj[num]);

	switch (obj->state)
	{
	case sSTART:
		if (GetEventParm(obj->xorg/(32*65536),obj->yorg/(32*65536),0,1))
			obj->loadanim=mCHUCK; //different anim!!!

		obj->curanim=AnimBase[obj->loadanim]+mBIRD_CAGE;
		obj->killanim=AnimBase[obj->loadanim]+mBIRD_CAGEBROK;

		obj->curframe=anims[obj->curanim].framelist[0];
		PutWalkerOnGround(num);
		obj->state=sSLEEP;
		break;

	case sSLEEP: //bird is in cage, don't do nothin'
		if (obj->counter==0)
		{
			if ((gamecounter&7)==0) obj->phase++;
			if (obj->phase>=anims[obj->curanim].numframes)
			{
				obj->phase=anims[obj->curanim].numframes-1;
				obj->counter=1;
			};
			obj->curframe=anims[obj->curanim].framelist[obj->phase];
			DrawGameobj(num);
		} else
		{
			obj->counter++;
			if (obj->counter>140+(int)RandFac(63))
			{
				obj->counter=0;
				obj->phase=0;
			};
			obj->curframe=anims[AnimBase[obj->loadanim]+mBIRD_CAGE].framelist[0];
			DrawGameobj(num);
		};
		break;

	case sFREEZE: 
        if (obj->freeze>0) obj->freeze--;
        if (obj->freeze==0) obj->state=obj->oldstate;
		DrawGameobj(num);
		break;	

	case sACTION: //player shot the cage
		cobj=AddObject(obj->xpos,obj->ypos,aBIRD,0);
		if (cobj>0)
		{
			obj2=(Omonster *)(&gameobj[cobj]);
			obj2->var1=obj->var1;
			obj2->loadanim=obj->loadanim; //chuck?

			birdfly(cobj); //display it!

			obj->nohit=1; //dont hit again
			obj->state=sDONE;
			obj->lighttype=0;
		}
		else
		{	//no more objects could be added! argh. dont destroy cage
			obj->state=sSLEEP;
		};
		break;	

	case sDONE:
		obj->curanim=AnimBase[obj->loadanim]+mBIRD_CAGEBROK;
		obj->curframe=anims[obj->curanim].framelist[0];
		DrawGameobj(num);
		break;

	case sKILL:
		cKILL(num);
		break;

	case sDEACTIVATE:
		//only can happen when still in cage!
	
		if (obj->curanim==AnimBase[obj->loadanim]+mBIRD_CAGE)
			cDEACTIVATE(num);
		else
		{	//fly too far away
			if ((obj->var1<PLAYSLOT) || (player[obj->var1-PLAYSLOT].bird==0))
				MyDeleteObject(num);//obj->code=NULL;
			else
			{
				obj->state=sDONE;
			};
		};
		break;

	default:
		FAIL("birdcage(): Illegal state: obj:%i, type:%i, state:%i\n",num,obj->load,obj->state);
		break;
	}; //switch

} //birdcage()


