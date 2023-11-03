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






