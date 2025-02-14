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


static int ricosamps[4]={sAMMO_BUL1,sAMMO_BULFL1,sAMMO_BULFL2,sAMMO_BULFL3};
