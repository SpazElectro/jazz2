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

/*
blah points costs

10000 devil devan|devil|devan

2000 LASER SHIELD|laser|shield
*/
void SetupEnemies(void)
{
	Omonster *obj;


//CHESHIRE1 (PSYCH LEV)
	obj=(Omonster *)(&loadobj[aCHESHIRE1]);
	obj->ProcPtr=&cheshire1;
	obj->objtype=oSPECIAL; //don't hurt J
	obj->nohit=1; //no coll
	//obj->energy=UNLIMITED;	//never kill
	obj->loadanim=mCAT;
	obj->curanim=AnimBase[mCAT]+mCAT_IDLE;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS2;

//CHESHIRE2 (PSYCH LEV)
	obj=(Omonster *)(&loadobj[aCHESHIRE2]);
	obj->ProcPtr=&cheshire2;
	obj->objtype=oSPECIAL; //don't hurt J
	obj->nohit=1;
	//obj->energy=UNLIMITED;
	obj->loadanim=mCAT2;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS2;
	obj->curanim=AnimBase[mCAT2]+mCAT2_IDLE;

//HATTER (PSYCH LEV)
	obj=(Omonster *)(&loadobj[aHATTER]);
	obj->xspeed=65536;
	obj->objtype=oNORMAL+oTRIGGERTNT+oENEMY;
	obj->ProcPtr=&hatter;
	obj->energy=3;
	obj->loadanim=mHATTER;
	obj->curanim=AnimBase[mHATTER]+mHATTER_WALK;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS2;
	obj->points=200;


//SKELETON
	obj=(Omonster *)(&loadobj[aSKELETON]);
	obj->xspeed=65536;
	obj->ProcPtr=&skeleton;
	obj->energy=3;
	obj->loadanim=mSKELETON;
	obj->curanim=AnimBase[mSKELETON]+mSKELETON_WALK;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS2;
	obj->objtype=oSPECIAL+oTRIGGERTNT+oENEMY;
	obj->points=200;

//DOGGY DOGG
	obj=(Omonster *)(&loadobj[aDOGGYDOGG]);
	obj->xspeed=65536;
	obj->ProcPtr=&dog;
	obj->energy=3;
	obj->loadanim=mDOG;
	obj->curanim=AnimBase[mDOG]+mDOG_WALK;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS2;
	obj->objtype=oSPECIAL+oTRIGGERTNT+oENEMY;
	obj->points=200;

//NORMTURTLE
	obj=(Omonster *)(&loadobj[aNORMTURTLE]);
	obj->objtype=oSPECIAL+oTRIGGERTNT+oENEMY;
	obj->xspeed=65536;
	obj->ProcPtr=&turtle;
	obj->energy=1;
	obj->loadanim=mTURTLE;
	obj->curanim=AnimBase[mTURTLE]+mTURTLE_WALK;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS2;
	obj->points=100;

//TURTLESHELL
	obj=(Omonster *)(&loadobj[aTURTLESHELL]);
	obj->objtype=oSPECIAL+oTRIGGERTNT;
	obj->ProcPtr=&turtleshell;
	obj->energy=2;
	obj->loadanim=mTURTLE;
	obj->curanim=AnimBase[mTURTLE]+mTURTLE_REVSHELL;
	obj->nohit=3;
	obj->points=100;
//	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS2;


//DEMON
	obj=(Omonster *)(&loadobj[aDEMON]);
	obj->ProcPtr=&demon;
	obj->xspeed=65536;
	obj->energy=2;
	obj->loadanim=mDEMON;
	obj->curanim=AnimBase[mDEMON]+mDEMON_RUNCYC;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS2;
	obj->points=100;
	obj->objtype=oNORMAL+oTRIGGERTNT+oENEMY;

//STEAM
	obj=(Omonster *)(&loadobj[aSTEAM]);
	obj->ProcPtr=&cSTEAM;
	obj->nohit=1;
	//obj->energy=UNLIMITED;
	obj->loadanim=mSTEAM;
	obj->objtype=oSPECIAL+oTRIGGERTNT;
	obj->curanim=AnimBase[mSTEAM]+mSTEAM_STEAM;

//ROCK
	obj=(Omonster *)(&loadobj[aROCK]);
	obj->ProcPtr=&cROCK;
	//obj->nohit=1;
	//obj->energy=UNLIMITED;
	obj->loadanim=mROCK;
	obj->objtype=oSPECIAL+oTRIGGERTNT+oALWAYSACTIVE;
	obj->curanim=AnimBase[mROCK]+mROCK_ROCK;



//HELMUT
	obj=(Omonster *)(&loadobj[aHELMUT]);
	obj->ProcPtr=&helmut;
	obj->xspeed=65536;
	obj->energy=1;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
	obj->loadanim=mHELMUT;
	obj->curanim=AnimBase[mHELMUT]+mHELMUT_WALK;
	obj->points=100;
	obj->objtype=oNORMAL+oTRIGGERTNT+oENEMY;


//BILSY BOSS
	obj=(Omonster *)(&loadobj[aBILSYBOSS]);
	obj->ProcPtr=&cBILSYBOSS;
	obj->energy=120;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
	obj->loadanim=mBILSBOSS;
	obj->curanim=AnimBase[mBILSBOSS]+mBILSBOSS_IDLE;
	obj->points=3000;
	obj->objtype=oSPECIAL+oTRIGGERTNT+oENEMY;
	obj->nohit=3;


//BAT
	obj=(Omonster *)(&loadobj[aBAT]);
	obj->ProcPtr=&bat;
	obj->energy=1;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
	obj->loadanim=mBAT;
	obj->curanim=AnimBase[mBAT]+mBAT_HANG;
	obj->points=100;
	obj->objtype=oNORMAL+oTRIGGERTNT+oENEMY;


//BUMBEE
	obj=(Omonster *)(&loadobj[aBUMBEE]);
	obj->ProcPtr=&bumbee;
	obj->energy=1;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
	obj->loadanim=mBUMBEE;
	obj->curanim=AnimBase[mBUMBEE]+mBUMBEE_FLY;
	obj->points=200;
	obj->objtype=oNORMAL+oTRIGGERTNT+oENEMY;


//DRAGFLY
	obj=(Omonster *)(&loadobj[aDRAGONFLY]);
	obj->ProcPtr=&dragfly;
	obj->energy=1;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
	obj->loadanim=mDRAGFLY;
	obj->curanim=AnimBase[mDRAGFLY]+mDRAGFLY_DRAGFLY;
	obj->points=200;
	obj->objtype=oNORMAL+oTRIGGERTNT+oENEMY;


//DRAGON
	obj=(Omonster *)(&loadobj[aDRAGON]);
	obj->ProcPtr=&dragon;
	obj->energy=1;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
	obj->loadanim=mDRAGON;
	obj->curanim=AnimBase[mDRAGON]+mDRAGON_STAND;
	obj->points=200;
	obj->objtype=oNORMAL+oTRIGGERTNT+oENEMY;


//FAT CHIQA
	obj=(Omonster *)(&loadobj[aFATCHK]);
	obj->ProcPtr=&fatchick;
	obj->xspeed=65536;
	obj->energy=3;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
	obj->loadanim=mFATCHK;
	obj->curanim=AnimBase[mFATCHK]+mFATCHK_WALKCYC;
	obj->points=200;
	obj->objtype=oNORMAL+oTRIGGERTNT+oENEMY;

//FENCER
	obj=(Omonster *)(&loadobj[aFENCER]);
	obj->ProcPtr=&fencer;
	obj->energy=3;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
	obj->loadanim=mFENCER;
	obj->curanim=AnimBase[mFENCER]+mFENCER_STNDIDLE;
	obj->objtype=oSPECIAL+oTRIGGERTNT+oENEMY;
	obj->points=400;

//FISH
	obj=(Omonster *)(&loadobj[aFISH]);
	obj->ProcPtr=&fish;
	obj->energy=1;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
	obj->loadanim=mFISH;
	obj->curanim=AnimBase[mFISH]+mFISH_SWIM;
	obj->points=100;
	obj->objtype=oNORMAL+oTRIGGERTNT+oENEMY;


//MOTH
	obj=(Omonster *)(&loadobj[aMOTH]);
	obj->ProcPtr=&moth;
	//obj->energy=0;
	obj->nohit=1;
	obj->objtype=oSPECIAL; //dont hurt player
	//obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
	obj->loadanim=mMOTH;
	obj->curanim=AnimBase[mMOTH]+mMOTH_RED;


//RAPIER
	obj=(Omonster *)(&loadobj[aRAPIER]);
	obj->ProcPtr=&rapier;
	obj->energy=2;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
	obj->loadanim=mRAPIER;
	obj->curanim=AnimBase[mRAPIER]+mRAPIER_IDLE;
	obj->points=300;
	obj->objtype=oNORMAL+oTRIGGERTNT+oENEMY;

//SPARK
	obj=(Omonster *)(&loadobj[aSPARK]);
	obj->ProcPtr=&sparks;
	obj->energy=1;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
	obj->loadanim=mSPARK;
	obj->curanim=AnimBase[mSPARK]+mSPARK_FLY;
	obj->points=100;
	obj->objtype=oNORMAL+oTRIGGERTNT+oENEMY;


//PIN LEFT PADDLE
	obj=(Omonster *)(&loadobj[aPINLEFTPADDLE]);
	obj->ProcPtr=&cPINPADDLE;
	obj->nohit=1;
	//obj->energy=UNLIMITED;
	obj->loadanim=mPINBALL;
	obj->objtype=oSELFCOLLISION;//oSPECIAL;
	obj->curanim=AnimBase[mPINBALL]+mPINBALL_PADDLE1;

//PIN RIGHT PADDLE
	obj=(Omonster *)(&loadobj[aPINRIGHTPADDLE]);
	obj->ProcPtr=&cPINPADDLE;
	obj->nohit=1;
	//obj->energy=UNLIMITED;
	obj->loadanim=mPINBALL;
	obj->objtype=oSELFCOLLISION;//oSPECIAL;
	obj->curanim=AnimBase[mPINBALL]+mPINBALL_PADDLE2;

//PIN 500 BUMP
	obj=(Omonster *)(&loadobj[aPIN500BUMP]);
	obj->ProcPtr=&cPINBUMP;
	obj->nohit=1;
	//obj->energy=UNLIMITED;
	obj->loadanim=mPINBALL;
	obj->objtype=oSPECIAL;
	obj->curanim=AnimBase[mPINBALL]+mPINBALL_BUMP1B;
	obj->points=500;

//PIN CARROT BUMP
	obj=(Omonster *)(&loadobj[aPINCARROTBUMP]);
	obj->ProcPtr=&cPINBUMP;
	obj->nohit=1;
	//obj->energy=UNLIMITED;
	obj->loadanim=mPINBALL;
	obj->objtype=oSPECIAL;
	obj->curanim=AnimBase[mPINBALL]+mPINBALL_CARBUMP2;
	obj->points=500;

//STEADY LIGHT
	obj=(Omonster *)(&loadobj[aSTEADYLIGHT]);
	obj->ProcPtr=&cSTEADYLIGHT;
	obj->nohit=1;
	//obj->energy=UNLIMITED;
	obj->objtype=oSPECIAL;
	obj->lighttype=3;
	obj->light=20;

//PULZE LIGHT
	obj=(Omonster *)(&loadobj[aPULZELIGHT]);
	obj->ProcPtr=&cPULZELIGHT;
	obj->nohit=1;
	//obj->energy=UNLIMITED;
	obj->objtype=oSPECIAL;
	obj->lighttype=3;

//FLICKER LIGHT
	obj=(Omonster *)(&loadobj[aFLICKERLIGHT]);
	obj->ProcPtr=&cFLICKERLIGHT;
	obj->nohit=1;
	//obj->energy=UNLIMITED;
	obj->objtype=oSPECIAL;
	obj->lighttype=4;


//QUEENBOSS
	obj=(Omonster *)(&loadobj[aQUEENBOSS]);
	obj->ProcPtr=&queenboss;
	obj->nohit=3+8; //no freezing
	obj->energy=100;
	obj->objtype=oSPECIAL+oTRIGGERTNT+oENEMY;
	obj->loadanim=mQUEEN;
	obj->curanim=AnimBase[mQUEEN]+mQUEEN_IDLE;
	obj->points=2000;
	
//FLOATSUCKER
	obj=(Omonster *)(&loadobj[aFLOATSUCKER]);
	obj->ProcPtr=&floatsucker;
	obj->xspeed=65536;
	obj->energy=4;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
	obj->loadanim=mSUCKER;
	obj->objtype=oSPECIAL+oTRIGGERTNT+oENEMY;
	obj->curanim=AnimBase[mSUCKER]+mSUCKER_FLOAT;
	obj->points=200;

//BRIDGE
	obj=(Omonster *)(&loadobj[aBRIDGE]);
	obj->ProcPtr=&bridge;
//	obj->energy=4;
	obj->nohit=1+8;
	obj->loadanim=mBRIDGE;
	obj->objtype=oSPECIAL+oALWAYSACTIVE;

//MONKEY
	obj=(Omonster *)(&loadobj[aMONKEY]);
	obj->ProcPtr=&monkey;
//	obj->var4=0;
	obj->energy=3;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
	obj->loadanim=mMONKEY;
	obj->curanim=AnimBase[mMONKEY]+mMONKEY_WALK;
	obj->points=200;
	obj->objtype=oNORMAL+oTRIGGERTNT+oENEMY;

//STANDMONKEY
	obj=(Omonster *)(&loadobj[aSTANDMONKEY]);
	obj->ProcPtr=&monkey;
	obj->var4=1;
	obj->energy=3;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
	obj->loadanim=mMONKEY;
	obj->curanim=AnimBase[mMONKEY]+mMONKEY_IDLE;
	obj->points=300;
	obj->objtype=oNORMAL+oTRIGGERTNT+oENEMY;


//DESCTRUCTABLE SCENERY
	obj=(Omonster *)(&loadobj[aDESTRUCTSCENERY]);
	obj->ProcPtr=&cDESTRUCTSCENERY;
	obj->objtype=oSPECIAL+oTRIGGERTNT;
	obj->loadanim=mDESTSCEN;
	obj->nohit=3; //total no hit!
	obj->curanim=AnimBase[mDESTSCEN]+mDESTSCEN_32;
	obj->points=50;

//DESCTRUCTABLE SCENERY, ONLY WITH BOMB
	obj=(Omonster *)(&loadobj[aDESTRUCTSCENERYBOMB]);
	obj->ProcPtr=&cDESTRUCTSCENERY;
	obj->objtype=oSPECIAL+oTRIGGERTNT;
	obj->nohit=3; //nohit, custom hit code
	obj->loadanim=mDESTSCEN;
	obj->curanim=AnimBase[mDESTSCEN]+mDESTSCEN_32;
	obj->points=200;

//COLLAPSING SCENERY
	obj=(Omonster *)(&loadobj[aCOLLAPSESCENERY]);
	obj->ProcPtr=&cDESTRUCTSCENERY;
	obj->objtype=oSPECIAL+oTRIGGERTNT;
	obj->nohit=3; //nohit, custom hit code
	obj->loadanim=mDESTSCEN;
	obj->curanim=AnimBase[mDESTSCEN]+mDESTSCEN_32;

//BUTTSTOMPING COLLAPSABLE SCENERY
	obj=(Omonster *)(&loadobj[aSTOMPSCENERY]);
	obj->ProcPtr=&cDESTRUCTSCENERY;
	obj->objtype=oSPECIAL+oTRIGGERTNT;
	obj->nohit=3; //nohit, bullet flies on
	obj->loadanim=mDESTSCEN;
	obj->curanim=AnimBase[mDESTSCEN]+mDESTSCEN_32;
	obj->points=50;

//TRIGGER SCENERY
	obj=(Omonster *)(&loadobj[aTRIGGERSCENERY]);
	obj->ProcPtr=&cTRIGGERSCENERY;
	obj->objtype=oSPECIAL;
	//obj->loadanim=mDESTSCEN;
	obj->nohit=3; //total no hit!
	//obj->curanim=AnimBase[mDESTSCEN]+mDESTSCEN_32;


//GEMSTOMP
	obj=(Omonster *)(&loadobj[aGEMSTOMP]);
	obj->ProcPtr=&cGEMSTOMP;
	obj->objtype=oSPECIAL+oTRIGGERTNT;
	obj->nohit=1; //nohit, bullet flies on
	//obj->energy=0;
	obj->loadanim=mDESTSCEN;
	obj->curanim=AnimBase[mDESTSCEN]+mDESTSCEN_32;

//RAVEN
	obj=(Omonster *)(&loadobj[aRAVEN]);
	obj->ProcPtr=&raven;
//	obj->xspeed=65536*2;
	obj->energy=2;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
	obj->loadanim=mRAVEN;
	obj->curanim=AnimBase[mRAVEN]+mRAVEN_FLYCYC;
	obj->points=300;
	obj->objtype=oNORMAL+oTRIGGERTNT+oENEMY;

//TUBETURTLE
	obj=(Omonster *)(&loadobj[aTUBETURTLE]);
	obj->ProcPtr=&tubeturtle;
	obj->xspeed=65536*2;
	obj->energy=2;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
	obj->loadanim=mTUBETURT;
	obj->objtype=oSPECIAL+oTRIGGERTNT+oENEMY;
	obj->curanim=AnimBase[mTUBETURT]+mTUBETURT_RAFT;
	obj->points=200;

//GEMRING 
	obj=(Omonster *)(&loadobj[aGEMRING]);
	obj->ProcPtr=&gemring;
	obj->nohit=1;
	obj->loadanim=mGEMRING;
	obj->objtype=oSPECIAL+oTRIGGERTNT;
	obj->curanim=AnimBase[mGEMRING]+mGEMRING_GEM;
	obj->points=1000;

//ROTATING SMALL TREE
	obj=(Omonster *)(&loadobj[aROTSMALLTREE]);
	obj->ProcPtr=&rottree;
//	obj->nohit=1;
	obj->loadanim=mSMALTREE;
	obj->objtype=oSPECIAL+oTRIGGERTNT+oALWAYSACTIVE;
	obj->curanim=AnimBase[mSMALTREE]+mSMALTREE_TREE;

//DIAMONDUS POLE
	obj=(Omonster *)(&loadobj[aDIAMONDUSPOLE]);
	obj->ProcPtr=&rottree;
//	obj->nohit=1;
	obj->loadanim=mDIAMPOLE;
	obj->objtype=oSPECIAL+oTRIGGERTNT+oALWAYSACTIVE;
	obj->curanim=AnimBase[mDIAMPOLE]+mDIAMPOLE_POLE;

//PSYCH POLE
	obj=(Omonster *)(&loadobj[aPSYCHPOLE]);
	obj->ProcPtr=&rottree;
//	obj->nohit=1;
	obj->loadanim=mPSYCHPOLE;
	obj->objtype=oSPECIAL+oTRIGGERTNT+oALWAYSACTIVE;
	obj->curanim=AnimBase[mPSYCHPOLE]+mPSYCHPOLE_POLE;


}	//SetupEnemies()






void SetupEnemies2(void)
{
	Omonster *obj;

//CRAB
	obj=(Omonster *)(&loadobj[aCRAB]);
	obj->ProcPtr=&cCRAB;
	obj->energy=3;
	obj->xspeed=65536;
	obj->loadanim=mUTERUS;
	//obj->objtype=oSPECIAL;
	obj->curanim=AnimBase[mUTERUS]+mUTERUS_WALK;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS2;
	obj->points=300;
	obj->objtype=oNORMAL+oTRIGGERTNT+oENEMY;


//ROBOT
	obj=(Omonster *)(&loadobj[aROBOT]);
	obj->ProcPtr=&cROBOT;
	obj->energy=100;
	obj->xspeed=65536;
	obj->loadanim=mROBOT;
	obj->objtype=oSPECIAL+oTRIGGERTNT+oENEMY;
	obj->curanim=AnimBase[mROBOT]+mROBOT_FLY;
	obj->points=2000;

//DEVANROBOT
	obj=(Omonster *)(&loadobj[aDEVANROBOT]);
	obj->ProcPtr=&cDEVANROBOT;
	obj->nohit=3;
	obj->energy=50;
	obj->loadanim=mDEVAN;
	obj->objtype=oSPECIAL+oTRIGGERTNT+oENEMY;
	obj->curanim=AnimBase[mDEVAN]+mDEVAN_CONTROL;
	obj->points=4000;


//FRUIT PLATFORM
	obj=(Omonster *)(&loadobj[aFRUITPLATFORM]);
	obj->ProcPtr=&CirclePlatform;
	obj->nohit=3;
	obj->energy=30;
	obj->loadanim=mFRUITPLAT;
	obj->objtype=oSPECIAL;
	obj->curanim=AnimBase[mFRUITPLAT]+mFRUITPLAT_PLAT;
	obj->killanim=AnimBase[mFRUITPLAT]+mFRUITPLAT_RING; //patch it in here ;-)


//BOLL PLATFORM
	obj=(Omonster *)(&loadobj[aBOLLPLATFORM]);
	obj->ProcPtr=&CirclePlatform;
	obj->nohit=3;
	obj->energy=30;
	obj->loadanim=mBOLLPLAT;
	obj->objtype=oSPECIAL;
	obj->curanim=AnimBase[mBOLLPLAT]+mBOLLPLAT_PLAT;
	obj->killanim=AnimBase[mBOLLPLAT]+mBOLLPLAT_RING; //patch it in here ;-)


//GRASS PLATFORM
	obj=(Omonster *)(&loadobj[aGRASSPLATFORM]);
	obj->ProcPtr=&CirclePlatform;
	obj->nohit=3;
	obj->energy=30;
	obj->loadanim=mGRASSPLAT;
	obj->objtype=oSPECIAL;
	obj->curanim=AnimBase[mGRASSPLAT]+mGRASSPLAT_PLAT;
	obj->killanim=AnimBase[mGRASSPLAT]+mGRASSPLAT_RING; //patch it in here ;-)

//PINK PLATFORM
	obj=(Omonster *)(&loadobj[aPINKPLATFORM]);
	obj->ProcPtr=&CirclePlatform;
	obj->nohit=3;
	obj->energy=30;
	obj->loadanim=mPINKPLAT;
	obj->objtype=oSPECIAL;
	obj->curanim=AnimBase[mPINKPLAT]+mPINKPLAT_PLAT;
	obj->killanim=AnimBase[mPINKPLAT]+mPINKPLAT_RING; //patch it in here ;-)

//SONIC PLATFORM
	obj=(Omonster *)(&loadobj[aSONICPLATFORM]);
	obj->ProcPtr=&CirclePlatform;
	obj->nohit=3;
	obj->energy=30;
	obj->loadanim=mSONICPLAT;
	obj->objtype=oSPECIAL;
	obj->curanim=AnimBase[mSONICPLAT]+mSONICPLAT_PLAT;
	obj->killanim=AnimBase[mSONICPLAT]+mSONICPLAT_RING; //patch it in here ;-)


//SPIKE PLATFORM
	obj=(Omonster *)(&loadobj[aSPIKEPLATFORM]);
	obj->ProcPtr=&CirclePlatform;
	obj->nohit=3;
	obj->energy=10;
	obj->loadanim=mSPIKEPLAT;
	obj->objtype=oSPECIAL;
	obj->curanim=AnimBase[mSPIKEPLAT]+mSPIKEPLAT_PLAT;
	obj->killanim=AnimBase[mSPIKEPLAT]+mSPIKEPLAT_RING; //patch it in here ;-)

//SPIKE BOLL
	obj=(Omonster *)(&loadobj[aSPIKEBOLL]);
	obj->ProcPtr=&CirclePlatform;
	obj->nohit=3;
	obj->energy=8;
	obj->loadanim=mSPIKEBOLL;
	obj->objtype=oSPECIAL;
	obj->curanim=AnimBase[mSPIKEBOLL]+mSPIKEBOLL_BOLL;
	obj->killanim=AnimBase[mSPIKEBOLL]+mSPIKEBOLL_RING; //patch it in here ;-)

//SPIKE BOLL 3D
	obj=(Omonster *)(&loadobj[aSPIKEBOLL3D]);
	obj->ProcPtr=&Swinger3D;
	obj->nohit=3;
	obj->energy=8;
	obj->loadanim=mSPIKEBOLL3D;
	obj->objtype=oSPECIAL;
	obj->curanim=AnimBase[mSPIKEBOLL3D]+mSPIKEBOLL3D_BOLL;
	obj->killanim=AnimBase[mSPIKEBOLL3D]+mSPIKEBOLL3D_RING; //patch it in here ;-)


//GENERATOR
	obj=(Omonster *)(&loadobj[aGENERATOR]);
	obj->ProcPtr=&cGENERATOR;
	obj->nohit=3;
	obj->energy=30;
	obj->objtype=oSPECIAL;

//EVA
	obj=(Omonster *)(&loadobj[aEVA]);
	obj->ProcPtr=&cEVA;
	obj->nohit=8+1; //not at all: bullets fly through
	obj->energy=30;
	obj->loadanim=mEVA;
	obj->objtype=oSPECIAL;
	obj->curanim=AnimBase[mEVA]+mEVA_IDLE;


//SPRING CORD
	obj=(Omonster *)(&loadobj[aSPRINGCORD]);
	obj->ProcPtr=0;

//BUMBEE
	obj=(Omonster *)(&loadobj[aBEES]);
	obj->ProcPtr=&bees;
	obj->energy=1;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
	obj->loadanim=mBEES;
	obj->curanim=AnimBase[mBEES]+mBEES_FLY;
	obj->points=100;
	obj->objtype=oNORMAL+oTRIGGERTNT+oENEMY;

//WITCH
	obj=(Omonster *)(&loadobj[aWITCH]);
	obj->ProcPtr=&cWITCH;
	obj->energy=30;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
	obj->loadanim=mWITCH;
	obj->curanim=AnimBase[mWITCH]+mWITCH_FLY;
	obj->killanim=AnimBase[mWITCH]+mWITCH_DIE;
	obj->points=1000;
	obj->objtype=oNORMAL+oTRIGGERTNT+oENEMY;

//ROCKET TURTLE
	obj=(Omonster *)(&loadobj[aROCKTURT]);
	obj->ProcPtr=&cROCKETTURTLE;
	obj->energy=100;
	obj->nohit=3;	//processed in special loop
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
	obj->loadanim=mROCKTURT;
	obj->curanim=AnimBase[mROCKTURT]+mROCKTURT_FLOAT;
	obj->points=2000;
	obj->objtype=oSPECIAL+oTRIGGERTNT+oENEMY;

//BUBBA
	obj=(Omonster *)(&loadobj[aBUBBA]);
	obj->ProcPtr=&cBUBBA;
	obj->energy=60+level.difficulty*32;
	obj->nohit=3;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
	obj->loadanim=mBUBBA;
	obj->curanim=AnimBase[mBUBBA]+mBUBBA_ATTACK;
	obj->points=4000;
	obj->objtype=oSPECIAL+oTRIGGERTNT+oENEMY;

//TWEEDLE
	obj=(Omonster *)(&loadobj[aTWEEDLEBOSS]);
	obj->ProcPtr=&cTWEEDLEBOSS;
	obj->energy=100;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
	obj->loadanim=mTWEEDLE;
	obj->curanim=AnimBase[mTWEEDLE]+mTWEEDLE_STAND;
	obj->points=2000;
	obj->objtype=oNORMAL+oTRIGGERTNT+oENEMY;

//DEVILDEVAN
	obj=(Omonster *)(&loadobj[aDEVILDEVAN]);
	obj->ProcPtr=&cDEVILDEVAN;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
	obj->loadanim=mDEVILDEVAN;
	obj->curanim=AnimBase[mDEVILDEVAN]+mDEVILDEVAN_STAND;
	obj->points=10000;
	obj->objtype=oSPECIAL+oTRIGGERTNT+oENEMY;
	obj->nohit=3+8;	//special collision, no freeze
	obj->energy=100;
	obj->var1=obj->energy*2;



//TUFBOSS
	obj=(Omonster *)(&loadobj[aTUFBOSS]);
	obj->ProcPtr=&cTUFBOSS;
	obj->energy=100;
	obj->nohit=3;	//processed in special loop
	obj->xspeed=32768*3;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
	obj->loadanim=mTUFBOSS;
	obj->curanim=AnimBase[mTUFBOSS]+mTUFBOSS_WALK;
	obj->points=5000;
	obj->objtype=oSPECIAL+oTRIGGERTNT+oENEMY;



//STOPWATCH
	obj=(Omonster *)(&loadobj[aSTOPWATCH]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_STPWCH;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;

//AREA BONUS ANIMATION!
	obj=(Omonster *)(&loadobj[areaWARP]);
	obj->ProcPtr=&cWARP;
	obj->objtype=oSPECIAL;
	obj->direction=1;
	obj->nohit=1+8+16;

//BIG ROCK
	obj=(Omonster *)(&loadobj[aBIGROCK]);
	obj->ProcPtr=&cBIGOBJECT;
	obj->var3=0; //no ammo given!
	obj->objtype=oSPECIAL;
	obj->energy=32;
	obj->nohit=3+16;
	obj->direction=1;
	obj->loadanim=mBIGROCK;
	obj->curanim=AnimBase[mBIGROCK]+mBIGROCK_BIGROCK;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS2;

//BIG BOXX
	obj=(Omonster *)(&loadobj[aBIGBOX]);
	obj->ProcPtr=&cBIGOBJECT;
	obj->var3=0; //no ammo given!
	obj->objtype=oSPECIAL;
	obj->energy=32;
	obj->nohit=3+16;
	obj->direction=1;
	obj->loadanim=mBIGBOX;
	obj->curanim=AnimBase[mBIGBOX]+mBIGBOX_BIGBOX;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS2;



//SONIC BOSS
	obj=(Omonster *)(&loadobj[aSONICBOSS]);
	obj->ProcPtr=&cSONICBOSS;
	obj->energy=80;
	obj->loadanim=mSONCSHIP;
	obj->curanim=AnimBase[mSONCSHIP]+mSONCSHIP_SHIPTOP;
	obj->points=3000;
	obj->objtype=oSPECIAL+oTRIGGERTNT+oENEMY;
	obj->nohit=3;


//BUTTERFLY
	obj=(Omonster *)(&loadobj[aBUTTERFLY]);
	obj->ProcPtr=&cBUTTERFLY;
	obj->energy=1;
	obj->loadanim=mBUTTERFLY;
	obj->curanim=AnimBase[mBUTTERFLY]+mBUTTERFLY_BTTRFLY;
	obj->points=10;
	obj->objtype=oNORMAL+oTRIGGERTNT+oENEMY;

//BEEBOY
	obj=(Omonster *)(&loadobj[aBEEBOY]);
	obj->ProcPtr=&cBEEBOY;
	obj->energy=1;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
	obj->loadanim=mBEEBOY;
	obj->curanim=AnimBase[mBEEBOY]+mBEEBOY_FLY;
	obj->points=200;
	obj->objtype=oNORMAL+oTRIGGERTNT+oENEMY;

//SNOW
	obj=(Omonster *)(&loadobj[aSNOW]);
	obj->ProcPtr=&cSNOW;
	obj->energy=1;
	obj->loadanim=mSNOW;
	obj->objtype=oSPECIAL;


//FLAG
	obj=(Omonster *)(&loadobj[aFLAG]);
	obj->ProcPtr=&cFLAG;
	obj->energy=127;
	obj->loadanim=mFLAG;
	obj->killanim=obj->curanim=AnimBase[mFLAG]+mFLAG_RED;
	obj->points=2000;
	obj->nohit=1+8;	//bullet doesnt collides, doesnt do damage + NO FREEZE
	obj->objtype=oSPECIAL;
	obj->var1=0;


} //SetupEnemies()


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
