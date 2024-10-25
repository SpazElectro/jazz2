/*
	G_BOXES.C

	routines for collision with solid boxes
*/
#include "G_Boxes.h"
#include "Random.h"
#include "E_Loaders.h"
#include "G_PlayerAnim.h"
#include "G_Collision.h"
#include "G_Ai.h"
#include "G_Bullet.h"
#include "G_Objects.h"
#include "Globals.h"

#define PUSHSPEED (3*8192)
#define BOXFUDGEX (6*8192)

void ClearPlayerPlatform(int numo)
{
	int p;

	for (p=0;p<MAXPLAYERS;p++)
	if (player[p].Active)
	{
		if (player[p].platform==numo)
		{
			player[p].platform=0;
			player[p].platformtype=0;
			player[p].movexspeed=0;
			player[p].moveyspeed=0;
		}
	}	//for p
} //ClearPlayerPlatform()



/*
	returns +1 for collision on the left side
	returns -1 right side
	returns -2 for top side
	0 for no collision
*/
int CheckPlayerSolidBox(int numo)
{
	Tgameobj *obj;
	Tplayer *play;
	int frame;
	int x0,y0,x1,y1;
	int p;
	int hit;
	int hitsides;
//	int hitabove;
	int platformhit;
	int width,height;

	obj=&gameobj[numo];


	frame=anims[obj->curanim].framelist[obj->phase];
#if _DEBUG
	if (frame==0)
		DebugXMessage("frame0 in box");
#endif

	width=frames[frame].dx*32768;
	height=frames[frame].dy*32768;


	x0=obj->xpos-width-16*65536;
	y0=obj->ypos+frames[frame].ty2*65536-18*65536; //18=distance between playermid and playerfeet

	x1=obj->xpos+width+16*65536;
	y1=y0+height*2;


	hitsides=0;
	for (p=0;p<MAXPLAYERS;p++)
	if (player[p].Active)
	{
		play=&player[p];

		hit=0;
		platformhit=0;

//PLAYER ON THE SIDES???
		if ((play->ypos>y0+65536) && (play->ypos<y1) &&
			(play->xpos>x0-width) && (play->xpos<x1+width))
		{
			if ((play->xspeed>=0) && (play->xpos<=x0) && (play->xpos+play->xspeed>x0-BOXFUDGEX))
			{
				play->xpos=x0-BOXFUDGEX+PUSHSPEED;
				play->xspeed=0;
				if (play->xacc>0) play->xacc=0; //not really necessary
				hitsides+=1;
				hit=1;
			}
			if ((play->xspeed<=0) && (play->xpos>=x1) && (play->xpos+play->xspeed<x1+BOXFUDGEX))
			{
				play->xpos=x1+BOXFUDGEX-PUSHSPEED;
				play->xspeed=0;
				if (play->xacc<0) play->xacc=0; //not really necessary
				hitsides-=1;
				hit=1;
			}
		} //if apx near

		if (hit)
		{
			play->pushobject=numo;
			if (play->push<60) play->push=60;
			play->platform=numo;
			play->platformtype=0;	//still falling!
		} else
		if ((play->pushobject==numo) && (play->platform==numo))
		{
			play->pushobject=0;
			play->platform=0;
			play->platformtype=0;
			play->movexspeed=0;
			play->moveyspeed=0;
		}


//PLAYER ABOVE IT (STAND ON IT)
		if (!hit && (play->xpos>x0+BOXFUDGEX) && (play->xpos<x1-BOXFUDGEX))
		{
			if ((play->ypos>=y0) && 
				(play->ypos<(y0+y1)/2) && 
				(play->yspeed>-65536))
			{
				if (play->downattack<DOWNATTACKLEN)
					play->downattack=DOWNATTACKLEN;

				play->ypos=y0+32767; //round off
				play->platform=numo;
				play->platformtype=1; //no falling
				play->movexspeed=obj->xspeed; //no extra speed here
				play->moveyspeed=0;
				hit=-2;
				platformhit=1;
			}
		}

		if ((!hit) && (platformhit==0))
		{			
			if (play->platform==numo) //clear the platform number!
			{
				play->pushobject=0;
				play->platform=0;
				play->platformtype=0;
				play->movexspeed=0;
				play->moveyspeed=0;
			}
		}

	} //for p

	return(hitsides);
} //CheckPlayerSolidBox





void ClearSolidBox(int numo)
{
	int p;
	Tplayer *play;


	for (p=0;p<MAXPLAYERS;p++)
	if (player[p].Active)
	{
		play=&player[p];
		if ((play->pushobject==numo) || (play->platform==numo))
		{
			play->pushobject=0;
			play->platform=0;
			play->platformtype=0;
			play->movexspeed=0;
			play->moveyspeed=0;
		}
	}

	gameobj[numo].xspeed=0; //no more movement
} //ClearSolidBox()



void CollideSolidBox(int num,int *newxpos,int *newypos)
{
	int hitl,hitr;
	int n;
	Omonster *obj;
	int frame,dx,dy;
//	int speed=0;
	int width,height;


	obj=(Omonster *)(&gameobj[num]);

	frame=anims[obj->curanim].framelist[obj->phase];
	width=(frames[frame].dx-4)*32768;
	height=(frames[frame].dy-4)*32768;


	if (*newxpos<obj->xpos)
	{
		if (CheckPixel(*newxpos-width,*newypos))
			*newxpos=obj->xpos;
	} else
	if (*newxpos>obj->xpos)
	{
		if (CheckPixel(*newxpos+width,*newypos))
			*newxpos=obj->xpos;
	}

	for (n=1;n<NumObjects;n++)
	if (((gameobj[n].ProcPtr==boxx) ||
		(gameobj[n].ProcPtr==ammomonitor) ||
		(gameobj[n].ProcPtr==cBIGOBJECT) ||
		(gameobj[n].ProcPtr==monitor)) && 
		(n!=num))
	{
		dx=((frames[gameobj[n].curframe].dx-4)*32768)+width;
		dy=((frames[gameobj[n].curframe].dy-4)*32768)+height;
		if (ABS(gameobj[n].xpos-*newxpos)<dx)
		{
			if (ABS(gameobj[n].ypos-*newypos)<dy)
			{
				*newxpos=obj->xpos;
				*newypos=obj->ypos;
				break;
			}
		}
	}

	
	dy=(frames[frame].ty2+frames[frame].dy)*65536;
	hitl=CheckPixel(*newxpos-width,*newypos+dy);
	hitr=CheckPixel(*newxpos+width,*newypos+dy);

	if ((!hitl) && (!hitr))
	{
		obj->state=sFALL;
	}
	else
	{
		if (obj->freeze)
			obj->state=sFREEZE;
		else
			obj->state=sSLEEP;
	}
} //CollideSolidBox()



int PushSolidBox(int num,int direction)
{
	int hitl,hitr;
	int n;
	Omonster *obj;
	int frame,dx,dy;
	int newxpos=0;
	int speed=0;
	int width,height;

	obj=(Omonster *)(&gameobj[num]);

	frame=anims[obj->curanim].framelist[obj->phase];
	width=(frames[frame].dx-4)*32768;
	height=(frames[frame].dy-4)*32768;


	if (direction<0)
	{
		if (!CheckPixel(obj->xpos-width,obj->ypos))
			newxpos=obj->xpos-PUSHSPEED;
	} else
	if (direction>0)
	{
		if (!CheckPixel(obj->xpos+width,obj->ypos))
			newxpos=obj->xpos+PUSHSPEED;
	}

	if (newxpos)	//only when moved
	{
		for (n=1;n<NumObjects;n++)
		if (((gameobj[n].ProcPtr==boxx) ||
			(gameobj[n].ProcPtr==ammomonitor) ||
			(gameobj[n].ProcPtr==cBIGOBJECT) ||
			(gameobj[n].ProcPtr==monitor)) && 
			(n!=num))
		{
			dx=((frames[gameobj[n].curframe].dx-4)*32768)+width;
			if (ABS(gameobj[n].xpos-newxpos)<dx)
			{
				dy=((frames[gameobj[n].curframe].dy-4)*32768)+height;
				if (ABS(gameobj[n].ypos-obj->ypos)<dy)
				{
					newxpos=0;
					break;
				}
			}
		}

		if (newxpos)
		{
			speed=newxpos-obj->xpos;
			obj->xpos=newxpos;
		}
	}
	

//	px=obj->xpos+(frames[obj->curframe].tx2*65536);
//	py=(frames[obj->curframe].ty2+frames[obj->curframe].dy)*65536;
//	hitdx=(frames[obj->curframe].dx*65536)/2;

	
	dy=(frames[frame].ty2+frames[frame].dy)*65536;
	hitl=CheckPixel(obj->xpos-width,obj->ypos+dy);
	hitr=CheckPixel(obj->xpos+width,obj->ypos+dy);

	if ((!hitl) && (!hitr))
	{
		obj->state=sFALL;
	}
	else
	{
		if (RandFac(15)==0)
		{
			if ((direction<0) && hitr)
				AddExplosion(obj->xpos+width,obj->ypos+dy,AnimBase[mAMMO]+mAMMO_POOF2); 
			else
			if ((direction>0) && hitl)
				AddExplosion(obj->xpos-width,obj->ypos+dy,AnimBase[mAMMO]+mAMMO_POOF2); 
		}


		if (obj->freeze)
			obj->state=sFREEZE;
		else
			obj->state=sSLEEP;
	}

	return(speed);
} //PushSolidBox()


