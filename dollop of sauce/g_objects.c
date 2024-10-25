/*
	G_AIOBJECTS.C


	object handling routines, main object loop,
	object management (kill, add, check)

*/
#include "G_Objects.h"
#include "G_Event.h"
#include "G_Ai.h"
#include "G_Collision.h"
#include "G_Bullet.h"
#include "G_PlayerColl.h"
#include "G_AiTypes.h"
#include "Globals.h"
#include "Grade.h"
#include "N_Main.h"

//globals 
int FirstFreeObject;

int NumObjects;

int MAXOBJECTS;




void CheckObject(int num) {
	Tgameobj *obj; // The targeted game object
	int event,calc;



	obj=&gameobj[num]; // Put a reference on the object in the array

	if (!(obj->objtype & oALWAYSACTIVE)) {

		if ((MultiPlayerConnection>gameLOCAL) || (MultiPlayerType>gameCOOPERATIVE)) {
			if (!CheckInLevel(obj->xpos,obj->ypos)) obj->state=sDEACTIVATE;
		}
		else
		{
			if ((!InViewArea(obj->xpos,obj->ypos)) || (!CheckInLevel(obj->xpos,obj->ypos))) obj->state=sDEACTIVATE;
		}
	}


	if (obj->justhit)
		obj->justhit--;

	if ((obj->objtype&oTYPES)==oPLAYERBULLET)
	{
		//wait for it to update first!, and dont do collision with explosions
		//if not explicitely coded (makes it too easy to kill opponents)
		if ((obj->state!=sSTART) && (obj->state!=sEXPLODE))
			CheckBullet(num);
	}
	else
	{
		CheckPlayerObjectCollision(num);

///BAH HUMBUG
		event=GetEvent(obj->xpos/(32*65536),1+obj->ypos/(32*65536));
		if ((event>=areaBELTRIGHT) && (event<=areaBELTACCLEFT))
		{
			calc=GetEventParm(obj->xpos/(32*65536),obj->ypos/(32*65536),0,-8);
			if (calc==0)
				calc=2*65536;
			else
				calc*=65536;

			if (event==areaBELTRIGHT)
				obj->xpos+=calc;
			else
			if (event==areaBELTLEFT)
				obj->xpos-=calc;
			else
			if (event==areaBELTACCRIGHT)
			{
				obj->xpos+=calc;
				//obj->xspeed+=calc;
			}
			else
			if (event==areaBELTACCLEFT)
			{
				obj->xpos-=calc;
				//obj->xspeed-=calc;			
			}
		}
	}
} //CheckObject





void DoObjects(void)
{
	int num;

	ASSERT(gameobj);


	NumObjects=0;
	for (num=1;num<MAXOBJECTS;num++)
	if (gameobj[num].ProcPtr) {
		NumObjects=num;
	}

	NumObjects++;



	for (num=1;num<NumObjects;num++)
	if (gameobj[num].ProcPtr)
	{
		CheckObject(num);
		if (gameobj[num].ProcPtr) {	//can be deleted now!
			gameobj[num].ProcPtr(num);	//jump to code of object
		}
	} //for each object, check if active
} //DoObjects()





void KillObject(int objn)
{


	if (gameobj[objn].ProcPtr) {
		//set state to KILL
		gameobj[objn].state=sKILL;
		//jump to object code, let object kill itself	
		gameobj[objn].ProcPtr(objn);

		MyDeleteObject(objn);//gameobj[objn].code=NULL;
	}

} //KillObject()





int AddObject(int xpos,int ypos,int objn,int creator)
{
	int num;
	Tgameobj *obj;

#if _DEBUG
	if ((objn>=MAXLOADOBJECTS) && (objn<=0))
	{
		return(-1);
	}
#endif
	if (FirstFreeObject<MAXOBJECTS)
	{
		num=FirstFreeObject;
		if (gameobj[num].ProcPtr)
		{
			for (;num<MAXOBJECTS;num++)
			if (!gameobj[num].ProcPtr) {
				FirstFreeObject=num+1;
				goto FillIt;
			};
		} else
			FirstFreeObject++;
	} else
		num=MAXOBJECTS;

	if (num>=MAXOBJECTS)	//needed?
	{
		for (num=1;num<MAXOBJECTS;num++)
		if (!gameobj[num].ProcPtr)
		{
			FirstFreeObject=num+1;
			goto FillIt;
		} //try to find one this time!

		return(0); //argh
	}

FillIt:
	obj=&gameobj[num];
	FastMemCpy(obj,&loadobj[objn],128);

	obj->xorg=obj->xpos=xpos;
	obj->yorg=obj->ypos=ypos;
	obj->creator=creator;
#if _NET
	if (MultiPlayerConnection!=gameLOCAL)
		UID[num]=CreateUID(creator,objn,xpos,ypos);
#endif

	if (num>=NumObjects)
	{
		NumObjects=num+1;
	}
	return(num);
} //AddObject()




void MyDeleteObject(int num)
{

	if (num<FirstFreeObject)
		FirstFreeObject=num;

	ASSERT(num<MAXOBJECTS);

	gameobj[num].ProcPtr=0;

#if _NET
	UID[num]=0;
#endif
//	if (NumObjects==num+1)
//		NumObjects=num;		//scale back by ONE!

} //MyDeleteObject()



