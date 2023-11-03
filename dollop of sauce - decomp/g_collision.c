/*
	G_AICOLLISION.C

	AI collision utilities

*/
#include "G_Collision.h"
#include "E_Loaders.h"
#include "G_Event.h"
#include "Globals.h"
#include "E_Parallax.h"
#include "G_AiTypes.h"
#include "G_Objects.h"
#include "G_Ai.h"

int tileattr;		//global variable, getvline and gethline routines put the event type in here!




//deff
static const int SOLIDAREA[32]={9,1,1,0, 0,1,1,0, 1,1,1,1, 1,1,1,0,
								1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1};


/*
  deactivate:

  als deactivate gezet wordt terwijl de state nog START is, dan kan de code meteen
  op NULL gezet worden.

  als dit niet gedaan wordt: dan kan deactivate van special-objects dingen terugschrijven
  die nog niet initialised zijn.

  dit was de bug bij destructable scenery

*/



// Define-macros for bit manipulation (speedy)
#define GetBit(t,x,y)		((*(TileSet.Mask[(t)]+(y)) & (1<<(x)))!=0)
#define GetBits(t,x1,x2,y)	((*(TileSet.Mask[(t)]+(y)) & ((0xffffffff<<(x1)) & (0xffffffff>>(31-(x2)))))!=0)
#define GetLBits(t,x,y)		((*(TileSet.Mask[(t)]+(y)) & (0xffffffff>>(31-(x))))!=0)
#define GetRBits(t,x,y)		((*(TileSet.Mask[(t)]+(y)) & (0xffffffff<<(x)) )!=0)


int BCheckHline(int tile0,int tile1,int xs,int dx,int y)
{
	int check=0;

    if ((xs+dx)<32)
    {
        if (tile0)
			check=GetBits(tile0,xs,xs+dx,y);
	}
	else
	{
        if ((tile0) && (xs<32))
			check=GetRBits(tile0,xs,y);

		if ((tile1) && (!check))
			check=GetLBits(tile1,dx-(31-xs),y);
	}

	return(check);
} //BCheckHline()


int CheckHBlock(int tile0,int tile1,int tile2,int tile3,int xs,int ys)
{
	int yt;
	int check;

    yt=0;
	do{
		if ((ys+yt)<32)
			check=BCheckHline(tile0,tile1,xs,31,ys+yt);
		else
			check=BCheckHline(tile2,tile3,xs,31,ys+yt-32);

		yt++;
	} while ((check==0) && (yt<32) );

	if (check==0)
		yt=-1;

	return(yt);
} //CheckHBlock()

#define INVIEWACTIVEX	320*3*65536
#define INVIEWACTIVEY	200*4*65536




void PutObjectOnGround(int num)
{
	Tgameobj *obj;
	int px,py,hit;



	obj=&gameobj[num];

	px=obj->xpos+(((frames[obj->curframe].dx/2)+frames[obj->curframe].tx2)*65536);
	py=(frames[obj->curframe].ty2+frames[obj->curframe].dy)*65536;

	do{

		hit=CheckPixel(px,obj->ypos+py);
		if (!hit)
		{
			obj->ypos+=4*65536;
			if (obj->ypos>level.fixheight)
				return;
		}

	} while (hit==0);

} //PutObjectOnGround()



