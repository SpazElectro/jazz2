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


