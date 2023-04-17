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



   

int CheckPixel(int x, int y)
{
	int tile;
	int xPixel = x >> 16;
	int yPixel = y >> 16;
	int xTile  = xPixel >> 5;
	int yTile  = yPixel >> 5;
	int res;

	if (((Word32)xPixel > (Word32)level.pixelwidth) || ((Word32)yPixel > (Word32)level.pixelheight))
	{
		return(-2);
	}

	tile=GetSpriteLayerTile(xTile, yTile);

	if (tile)
	{
		tileattr=GetEvent(xTile, yTile);
		//if ((tileattr&31) && (SOLIDAREA[tileattr&31]==0)) return 0; //check for attr

		res=GetBit(tile, (xPixel & 31), (yPixel & 31));
		return(res);
	}
	else
	{
		return(0);
	}
} //CheckPixel



int CheckFullPixel(int x,int y)
{
	int tile;
	int tx,ty;
	int res;

	tx=x/65536;
	ty=y/65536;

	if (((Word)tx>(Word)level.pixelwidth) ||
		((Word)ty>(Word)level.pixelheight))
	{
		return(-2);
	}


	tile=GetSpriteLayerTile(tx/32, ty/32);

	if (tile)
	{
		tileattr=GetEvent(tx/32,ty/32);

		if ((tileattr==areaONEWAY) ||
			(tileattr==areaVINE) ||
			(tileattr==areaHOOK))
		{
			return(0);
		}
		else
		{
			res=GetBit(tile,tx&31,ty&31);
			return(res);
		}
	}
	else
		return(0);

} //CheckFullPixel







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


int CheckHLine(int x1,int dx,int y)
{
	int blockx,blocky;
	int fractx,fracty;
	int tile;
	int x;
	int check;
	int xcheckend;

	x=x1;
	check=0;

	blocky=y/32;
	fracty=y&31;

    if ((blocky<0) ||
        (blocky>level.blockheight))
	{
        return(0);
	}

	do
	{
		fractx=x&31;
		blockx=x/32;
        if ((blockx<0) ||
            (blockx>(int)level.blockwidth))
            tile=0;
        else
			tile=GetSpriteLayerTile(blockx, blocky);
		
		if (tile)
			tileattr=GetEvent(blockx,blocky);

		xcheckend=fractx+dx;
		if (xcheckend>=31)
		{
			if (tile) check=GetRBits(tile,fractx,fracty);
			dx-=32-fractx;
			x+=32-fractx;	//should be 0 on next iter
		}
		else
		if (fractx==0)
		{
			if (tile) check=GetLBits(tile,xcheckend,fracty);
			dx=0; //we should stop after this
		}
		else
		{
			if (tile) check=GetBits(tile,fractx,xcheckend,fracty);
			dx=0; //we should stop after this
		};

	} while ((!check) && (dx>0));

	return(check);
} //CheckHLine()


//this one checks for REAL solid pixels inside, will exit
//only when it found a real pixel inside a block will attr 0
int TCheckHLine(int x1,int dx,int y)
{
	int blockx,blocky;
	int fractx,fracty;
	int tile;
	int x;
	int check;
	int xcheckend;

	x=x1;
	check=0;

	blocky=y/32;
	fracty=y&31;

    if ((blocky<0) ||
        (blocky>level.blockheight))
	{
        return(0);
	}


	do
	{
		fractx=x&31;
		blockx=x/32;
        if ((blockx<0) ||
            (blockx>(int)level.blockwidth))
            tile=0;
        else
		tile=GetSpriteLayerTile(blockx, blocky);

		if (tile)
		{
			tileattr=GetEvent(blockx,blocky);
			if ((tileattr) && 
				(tileattr<29) && 
				(SOLIDAREA[tileattr&31]==0)) tile=0;
		}

		xcheckend=fractx+dx;
		if (xcheckend>=31)
		{
			if (tile) check=GetRBits(tile,fractx,fracty);
			dx-=32-fractx;
			x+=32-fractx;	//should be 0 on next iter
		}
		else
		if (fractx==0)
		{
			if (tile) check=GetLBits(tile,xcheckend,fracty);
			dx=0; //we should stop after this
		}
		else
		{
			if (tile) check=GetBits(tile,fractx,xcheckend,fracty);
			dx=0; //we should stop after this
		}

	} while ((!check) && (dx>0));

	return(check);
} //TCheckHLine()



int CheckVLine(int x,int y1,int dy)
{
	int blockx,blocky;
	int fractx,fracty;
	int tile;
	int y;
	int y2;
	int check;
	int start;

	y=y1; y2=y1+dy;
	start=1;
	check=0;

	blockx=x/32; fractx=x&31;
	blocky=y/32;

    if ((blockx<0) ||
        (blockx>(int)level.blockwidth))
	{
		return(0);
	}


    do{
		fracty=y&31;
		if ((!fracty) || (start))
		{
			blocky=y/32;
            if ((blocky<0) ||
                (blocky>level.blockheight))
                tile=0;
            else
    		tile=GetSpriteLayerTile(blockx, blocky);

			if (tile)
			{
				start=0;
				tileattr=GetEvent(blockx,blocky);
			}
		}

		if (tile)
			check=GetBit(tile,fractx,fracty);

		y++;
	} while ((!check) && (y<=y2));

	return(y-y1);
} //CheckVLine()


int PixelCollision(Tgameobj *obj1,Tgameobj *obj2)
{
	Tframes *frame1;
	Tframes *frame2;
	int x1,y1,dx1,dy1;
	int x2,y2,dx2,dy2;
	int xb,xe,yb,ye;
	int pix1,pix2;
	int lx,ly;
	Word32 bofs1;
	Word32 bofs2;
	int dir1;
	int dir2;
    Word32 bit1;
    Word32 bit2;
    Word32 mask1;
    Word32 mask2;
	Word32 sx1,sx2;
	Word32 numhits=0;
	Word32 *maskptr1;
	Word32 *maskptr2;


	if ((obj1->curframe<=0) || (obj1->curframe>=MAXFRAMES) ||
		(obj2->curframe<=0) || (obj2->curframe>=MAXFRAMES))
		return(0); //0 frame, outta here

//	ASSERT(obj1->curframe<MAXFRAMES);
//	ASSERT(obj2->curframe<MAXFRAMES);

	frame1=&frames[obj1->curframe];
	frame2=&frames[obj2->curframe];

	if ((frame1->mask==NULL) || (frame2->mask==NULL))
	{
		return(0);
	}


 	x1=(obj1->xpos/65536); 
 	dx1=frame1->dx;
 	dir1=obj1->direction;

	y1=(obj1->ypos/65536)+frame1->ty2;
	dy1=frame1->dy;

	x2=(obj2->xpos/65536); 
	dx2=frame2->dx; 
	dir2=obj2->direction;
	
	y2=(obj2->ypos/65536)+frame2->ty2; 
	dy2=frame2->dy;

	if (dir1<0) 
	{
		x1-=frame1->tx2;
		sx1=x1-dx1;
	}
	else
	{
		dir1=1;		//override for dir2==0
		x1+=frame1->tx2;
		sx1=x1;
	}

	if (dir2<0)
	{
		x2-=frame2->tx2;
		sx2=x2-dx2;
	}
	else
	{
		dir2=1;	//override for dir2==0
		x2+=frame2->tx2;
		sx2=x2;
	}


	if (((sx1+dx1)>sx2) && (sx1<(sx2+dx2)) && 
	   ((y1+dy1)>y2) && (y1<(y2+dy2)))
	{	//TOUCHIN!
		if (sx1>sx2) xb=sx1; else xb=sx2;
		if ((sx1+dx1)<(sx2+dx2)) xe=sx1+dx1; else xe=sx2+dx2;

		if (y1>y2) yb=y1; else yb=y2;
		if ((y1+dy1)<(y2+dy2)) ye=y1+dy1; else ye=y2+dy2;


		if (dir1<0)
			bofs1=((yb-y1)*dx1)+x1-xb;
		else
			bofs1=((yb-y1)*dx1)-x1+xb;

		if (dir2<0)
			bofs2=((yb-y2)*dx2)+x2-xb;	
		else
			bofs2=((yb-y2)*dx2)-x2+xb;


		if ((dir1>0) && (dir2>0))
		{
			xe-=xb;
			for (ly=yb;ly<ye;ly++)
			{
				maskptr1=frame1->mask+(bofs1/32);
				maskptr2=frame2->mask+(bofs2/32);

				bit1=1<<(bofs1&31);
				bit2=1<<(bofs2&31);

				mask1=*maskptr1++;
				mask2=*maskptr2++;

				lx=xe;
				do {
					pix1=mask1&bit1;		//get bit (just !=0 is enough)
					pix2=mask2&bit2;		

					if (pix1 && pix2) {

						numhits++;	//HIT !
						if (numhits>7) goto EXITNOW;
					}

					bit1<<=1;				//shift test position
					if (bit1==0)			//shifted all out, read next
					{
						bit1=1;				//test first bit
						mask1=*maskptr1++;	//next maskofs
					}

					bit2<<=1;				//shift test position
					if (bit2==0)			//shifted all out
					{
						bit2=1;				//bit is now 1 (test first bit)
						mask2=*maskptr2++;	//next maskofs
					}
				} while (--lx>0);	//just a counter, not used inside the loop
				bofs1+=dx1;
				bofs2+=dx2;

			} //for ly
		}
		else
		if ((dir1>0) && (dir2<0))
		{
			xe-=xb;
			for (ly=yb;ly<ye;ly++)
			{
				maskptr1=frame1->mask+(bofs1/32);
				maskptr2=frame2->mask+(bofs2/32);

				bit1=1<<(bofs1&31);
				bit2=1<<(bofs2&31);

				mask1=*maskptr1++;
				mask2=*maskptr2--;

				lx=xe;
				do {
					pix1=mask1&bit1;		//get bit (just !=0 is enough)
					pix2=mask2&bit2;		
	
					if (pix1 && pix2) {
						numhits++;	//HIT !
						if (numhits>7) goto EXITNOW;
					}

					bit1<<=1;				//shift test position
					if (bit1==0)			//shifted all out, read next
					{
						bit1=1;				//test first bit
						mask1=*maskptr1++;	//next maskofs
					}

					bit2>>=1;				//shift test position
					if (bit2==0)			//shifted all out
					{
						bit2=1<<31;				//bit is now 1 (test first bit)
						mask2=*maskptr2--;	//next maskofs
					}
				} while (--lx>0);	//just a counter, not used inside the loop
				bofs1+=dx1;
				bofs2+=dx2;
			} //for ly
		}
		else
		if ((dir1<0) && (dir2>0))
		{
			xe-=xb;
			for (ly=yb;ly<ye;ly++)
			{
				maskptr1=frame1->mask+(bofs1/32);
				maskptr2=frame2->mask+(bofs2/32);

				bit1=1<<(bofs1&31);
				bit2=1<<(bofs2&31);

				mask1=*maskptr1--;
				mask2=*maskptr2++;

				lx=xe;
				do {
					pix1=mask1&bit1;		//get bit (just !=0 is enough)
					pix2=mask2&bit2;		
 	
					if (pix1 && pix2) {
						numhits++;	//HIT !
						if (numhits>7) goto EXITNOW;
					}

					bit1>>=1;				//shift test position
					if (bit1==0)			//shifted all out, read next
					{
						bit1=1<<31;			//test first bit
						mask1=*maskptr1--;	//next maskofs
					}

					bit2<<=1;				//shift test position
					if (bit2==0)			//shifted all out
					{
						bit2=1;				//bit is now 1 (test first bit)
						mask2=*maskptr2++;	//next maskofs
					}
				} while (--lx>0);	//just a counter, not used inside the loop
				bofs1+=dx1;
				bofs2+=dx2;
			} //for ly
		}
		else
		if ((dir1<0) && (dir2<0))
		{
			xe-=xb;
			for (ly=yb;ly<ye;ly++)
			{
				maskptr1=frame1->mask+(bofs1/32);
				maskptr2=frame2->mask+(bofs2/32);

				bit1=1<<(bofs1&31);
				bit2=1<<(bofs2&31);

				mask1=*maskptr1--;
				mask2=*maskptr2--;

				lx=xe;
				do {
					pix1=mask1&bit1;		//get bit (just !=0 is enough)
					pix2=mask2&bit2;		

					if (pix1 && pix2) {
						numhits++;	//HIT !
						if (numhits>7) goto EXITNOW;
					}

					bit1>>=1;				//shift test position
					if (bit1==0)			//shifted all out, read next
					{
						bit1=1<<31;			//test first bit
						mask1=*maskptr1--;	//next maskofs
					}

					bit2>>=1;				//shift test position
					if (bit2==0)			//shifted all out
					{
						bit2=1<<31;			//bit is now 1 (test first bit)
						mask2=*maskptr2--;	//next maskofs
					}
				} while (--lx>0);	//just a counter, not used inside the loop
				bofs1+=dx1;
				bofs2+=dx2;
			} //for ly
		}
EXITNOW:
		return(numhits);
	}	// else
	return(0);
} //PixelCollision()


int OnScreen(int x,int y)
{
	int nump;
	int px,py;
	int width,height;

	width=ScreenWidth*65536/2;
	height=ScreenHeight*65536/2;

//for all localplayers

	for (nump=0;nump<MAXPLAYERS;nump++)
	if (player[nump].Active && player[nump].localplayer)	
	{
		px=player[nump].xpos;
		py=player[nump].ypos;

		if (
			(x<(px-width)) ||
			(x>(px+width)) ||
			(y<(py-height)) ||
			(y>(py+height)))
			continue;
		else
		{
			return(1);
		}
	}
	return(0);
} //OnScreen()

	
#define INVIEWACTIVEX	320*3*65536
#define INVIEWACTIVEY	200*4*65536
int InViewArea(int x,int y)
{
	int nump;
	int px,py;

	for (nump=0;nump<MAXPLAYERS;nump++)
	if (player[nump].Active && player[nump].localplayer)	
	{
		px=player[nump].xpos;
		py=player[nump].ypos;

		if (
			(x<(px-INVIEWACTIVEX)) ||
			(x>(px+INVIEWACTIVEX)) ||
			(y<(py-INVIEWACTIVEY)) ||
			(y>(py+INVIEWACTIVEY)))
			continue;
		else
		{
			return(1);
		}
	}
	return(0);
} //InViewArea



int CheckInLevel(int x,int y)
{
	if ((x<-320*65536) || (y<-200*65536) || (x>=320*65536+level.fixwidth) || (y>=200*65536+level.fixheight))
		return 0;
	else
		return 1;
} //CheckInLevel()







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




void PutWalkerOnGround(int num)
{
	Tgameobj *obj;
	int px,py;


	obj=&gameobj[num];

	obj->curframe=anims[obj->curanim].framelist[0];

	obj->ypos-=15*65536; //start a bit higher 

	px=obj->xpos+(((frames[obj->curframe].dx/2)+frames[obj->curframe].tx2)*65536);
	py=(frames[obj->curframe].ty2-frames[obj->curframe].ty)*65536;

	// Move object down until we've hit something
	for (; obj->ypos<level.fixheight; obj->ypos+=65536)
		if (CheckPixel(px, obj->ypos+py)) break;

} //PutWalkerOnGround()




int CheckForSpring(int num,int x,int y)
{
	int t,dx,dy;
	Omonster *mons;

	for (t=1;t<NumObjects;t++)
	if (gameobj[t].ProcPtr==spring)
	{
		dx=x-gameobj[t].xpos;

		if ((dx>-8*65536) && (dx<8*65536))
		{
			dy=y-gameobj[t].ypos;

			if ((dy>-16*65536) && (dy<4*65536))
			{
				mons=(Omonster *)(&gameobj[t]);
				if ((mons->var3>=mons->var2) && (mons->xspeed==0))
				{
					mons->state=sSPRING;
					mons->special=num;
					mons->var3=0;
					return(t);
				}
			}
		}
	}
	return(0);
} //CheckForSpring()



