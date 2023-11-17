/*
	G_AISETUP.C

	AI utilities

	these fill in the LOADOBJ slots with the default object values
	for each object. This is used in G_EVENT to trigger new objects.

	for some reason this compiles REALLLY SLOW in VIsual C++ 5


	Created: 01-10-1995 by Arjan Brussee
*/
#include "G_AiSetup.h"
#include "E_Sprites2.h"
#include "E_Loaders.h"
#include "G_Event.h"
#include "G_AiTypes.h"
#include "G_Ai.h"
#include "G_Bosses.h"
#include "G_Objects.h"
#include "Globals.h"
#include "E_Parallax.h"


//forward declaration
void SetupEnemies2(void);

// Extremely long functions take extremely long to compile when global optimizations are on

void InitObjects(void)
{
	int diff;
	int x,y;
	Word32 event,found;
	Word8 eventbyte;
	Word32 sample;
	Word32 coins;
	Word32 calc;
	Word32 *pUID_Map;
	Word32 *pUID_Start;
	int flags;


	if (MultiPlayerType==gameNORMAL) {
		MAXOBJECTS=512;
	} else {
		MAXOBJECTS=512+64*MAXPLAYERS;
	}

	gameobj=(Tgameobj *)AllocAPointerClear(sizeof(Tgameobj)*MAXOBJECTS);
	UID=(Word16 *)AllocAPointer(sizeof(Word16)*MAXOBJECTS);
	FirstFreeObject=1;
	NumObjects=1;

	level.pixelwidth=(Layer_Width[SPRITELAYER])*32-1;
	level.pixelheight=(Layer_Height[SPRITELAYER])*32-1;

	level.blockwidth=(Layer_Width[SPRITELAYER]-1);
	level.blockheight=(Layer_Height[SPRITELAYER]-1);

	level.fixwidth=((Layer_Width[SPRITELAYER])*32-1)*65536;
	level.fixheight=((Layer_Height[SPRITELAYER])*32-1)*65536;

	level.gravity=8192;

//standard level starts!

	//FastMemSet(AnimBase,sizeof(AnimBase),0);


	LoadAnim(mFONT,FALSE);
	LoadAnim(mJAZZ,FALSE);
	LoadAnim(mSPAZ,FALSE);
	LoadAnim(mSPAZ2,FALSE);

	LoadAnim(mMENUSOUNDS,FALSE); //still need the sounds
	LoadAnim(mSPAZSOUNDS,FALSE); //still need the sounds
	LoadAnim(mJAZZSOUNDS,FALSE); //still need the sounds

	
	LoadAnim(mAMMO,FALSE);
	LoadAnim(mCOMMON,FALSE);
	LoadAnim(mBIRD,FALSE);
	LoadAnim(mPICKUPS,FALSE);
	LoadAnim(mSPRING,FALSE);
	LoadAnim(mFACES,FALSE);
	LoadAnim(mRUSH,FALSE);

	if (MultiPlayerType==gameCAPTURE)
		LoadAnim(mFLAG,FALSE);

/*
ok,ok:
	now we're going to find the events in this level,
	we discard the events that won't be displayed on our
	level.difficulty level
	we find the start of the level
	we load the animations of each existing event
*/

	pUID_Map=UID_Map=(Word32 *)AllocAPointer(16384*4);
	pUID_Start=UID_Start=(Word32 *)AllocAPointer((level.blockheight+4)*4);

	flags=NumObjectsInLevel=found=0;
    for (y=0;y<=level.blockheight;y++)
    {
		*pUID_Start++=NumObjectsInLevel;

	   	for (x=0;x<=(int)level.blockwidth;x++)
    	{
			event=EventMap[y][x];

			eventbyte=(Word8)(event&255);

			if (eventbyte)
			{
				NumObjectsInLevel++;

				*pUID_Map=(NumObjectsInLevel<<16)+x;


				//difficulty.......
				diff=event&(0x100+0x200);

				if ((MultiPlayerConnection==gameLOCAL) || (MultiPlayerType==gameCOOPERATIVE))
				{
					if (diff==0x100)
					{
						if (level.difficulty>0)
						{	//ONLY SHOW ON EASY
							event=0; eventbyte=0;
							EventMap[y][x]=0; //delete!
						}
					} else
					if (diff==0x200)
					{
						if (level.difficulty<2)
						{	//ONLY SHOW ON HARD
							event=0; eventbyte=0;
							EventMap[y][x]=0; //delete!
						}
					} else 
					if (diff==0x300)
					{
						if (NumPlayers==1)
						{	//ONLY SHOW IN MULTIPLAYER!
							event=0; eventbyte=0;
							EventMap[y][x]=0; //delete!
						}
					}
				}
				else
				{	//find illegal objects in multiplayer games
					int find;

					if (eventbyte==aGENERATOR)	//get object from generator!
						eventbyte=GetEventParm(x,y,0,8);

					for (find=0;find<sizeof(EvilObjects);find++) {
						if (eventbyte==EvilObjects[find]) {
							DebugXMessage("Object %i illegal in multiplayer game\n",eventbyte);
							event=0; eventbyte=0;
							EventMap[y][x]=0; //delete!
							break;
						}
					}

					if ((eventbyte==aFLAG) && (MultiPlayerConnection==gameCAPTURE))
					{
						flags++;
						if (flags>2) {
							Fatal("More then two flags in CAPTURE the FLAG level!");
						}
					}
				}

				if (eventbyte==aAMBIENTSOUND)
				{
					sample=GetEventParm(x,y,0,8);
					if (sample==0)
						sample=mWIND;
					else
					if (sample==1)
						sample=mCOMMON;
					else
					if (sample==2)
						sample=mSCIENCE;
//					else
//					if (sample==3)
//						sample=mWATER;

					if (AnimBase[sample]==0)
						LoadAnim(sample,FALSE);

				}
				else
				if (eventbyte==areaWARP)
				{
					coins=GetEventParm(x,y,8,8);

					if ((coins==10) && (AnimBase[mWARP10]==0))
                    	LoadAnim(mWARP10,FALSE);
					else
					if ((coins==20) && (AnimBase[mWARP20]==0))
                    	LoadAnim(mWARP20,FALSE);
					else
					if ((coins==50) && (AnimBase[mWARP50]==0))
                    	LoadAnim(mWARP50,FALSE);
					else
					if ((coins==100) && (AnimBase[mWARP100]==0))
                    	LoadAnim(mWARP100,FALSE);
				}
				else
				if (eventbyte==aGENERATOR)
				{
					sample=GetEventParm(x,y,0,8);
					if (AnimBase[loadobj[sample].loadanim]==0)
						LoadAnim(loadobj[sample].loadanim,FALSE);
				}
				else
				if (eventbyte==aMORPHFROG)
				{
					if (AnimBase[mFROG]==0)
						LoadAnim(mFROG,FALSE); //load frog when needed
				}
				else
				if (eventbyte==aBIRDCAGE)
				{
					calc=GetEventParm(x,y,0,1);
					if (calc==1)
					{
						if (AnimBase[mCHUCK]==0)
							LoadAnim(mCHUCK,FALSE);
					}
				}
				else
				if (eventbyte>31)	//real event, non-area fucker
				{
					//init_general in init.c sets up initial loadanims.
					//these stay there for the rest of the game, won't
					//be overwritten (in loadobj that is)
					//after loadanim the game-objects need to be re-initialized
					//with the right animbase numbers
					if (AnimBase[loadobj[eventbyte].loadanim]==0)
					{
                    	LoadAnim(loadobj[eventbyte].loadanim,FALSE);
					}

		   		}	//real event found
		    }	//eventbyte
	    }	//for x
	}	//for y
	*pUID_Start++=NumObjectsInLevel;
	UID_Map=(Word32*)ResizeAPointer(UID_Map,NumObjectsInLevel*4);


	KeyboardKbhit(); //increase loading animation response


	if ((AnimBase[mQUEEN]) ||
		(AnimBase[mROCKTURT]) ||
		(AnimBase[mBILSBOSS]) ||
		(AnimBase[mBUBBA]) ||
		(AnimBase[mTWEEDLE]) ||
		(AnimBase[mUTERUS]) ||
		(AnimBase[mDEVILDEVAN]) ||
		(AnimBase[mSONCSHIP]) ||
		(AnimBase[mTUFBOSS]) ||
		(AnimBase[mROBOT]))
	{	//boss loaded, load status bar ongein
		LoadAnim(mBOSS,FALSE);
	}

	if (AnimBase[mWITCH]) {	//witch can morf player into frog!
		if (AnimBase[mFROG]==0) {
			LoadAnim(mFROG,FALSE); //load frog when needed
		}
	}


//check which anims are TEXTURES, and unpack em

	UnPackTexture(mROCK,mROCK_ROCK);
	UnPackTexture(mGEMRING,mGEMRING_GEM);
	UnPackTexture(mSMALTREE,mSMALTREE_TREE);
	UnPackTexture(mUTERUS,mUTERUS_FLY);
	UnPackTexture(mUTERUS,mUTERUS_OPEN);
	UnPackTexture(mUTERUS,mUTERUS_HIT);
	UnPackTexture(mUTERUS,mUTERUS_CLOSE);
	UnPackTexture(mUTERUS,mUTERUS_PART);

	UnPackTexture(mCARROTPOLE,mCARROTPOLE_POLE);
	UnPackTexture(mPSYCHPOLE,mPSYCHPOLE_POLE);
	UnPackTexture(mDIAMPOLE,mDIAMPOLE_POLE);
	UnPackTexture(mJUNGLEPOLE,mJUNGLEPOLE_POLE);

	UnPackTexture(mSPIKEBOLL3D,mSPIKEBOLL3D_BOLL);
	UnPackTexture(mSPIKEBOLL3D,mSPIKEBOLL3D_RING);

	UnPackTexture(mSONCSHIP,mSONCSHIP_ROCKET);
	UnPackTexture(mBILSBOSS,mBILSBOSS_FIREBALL);

	UnPackTexture(mFLAG,mFLAG_ARROW);

	SetupBullets();	//set up another time, to load rite animbases and such
	SetupEnemies();


	for (x=0;x<MAXLOADOBJECTS;x++)
	{	//fill up the rest of the loadobj table!!!!!!!!!
		loadobj[x].load=x;
		loadobj[x].curframe=anims[loadobj[x].curanim].framelist[0];
	}

} //InitObjects()
