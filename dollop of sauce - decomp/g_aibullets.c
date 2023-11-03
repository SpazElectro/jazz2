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



