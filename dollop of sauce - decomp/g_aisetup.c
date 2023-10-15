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

int ReInitializingObjects; //flag, is on when a level is cleared, so
									//that objects can restore everything

static Word8 EvilObjects[]=
{
	aSWINGVINE,
	aBIRDCAGE,
//	aMORPHMONITOR,
//	aMAXWEAPON,
//	aAUTOFIRE,
//	aFASTFEET,
//	aEXTRALIFE,
	aENDOFLEVELPOST,
//	aSAVEPOST,
	aBONUSLEVELPOST,
	aBIRDMORPHMONITOR,
//	aTRIGGERCRATE,
	aFLYCARROT,
	aTUFTURT,
	aTUFBOSS,
	aLABRAT,
	aDRAGON,
	aLIZARD,
	aBUMBEE,
	aRAPIER,
	aSPARK,
	aBAT,
	aSUCKER,
//	aCATERPILLAR,
	aCHESHIRE1,
	aCHESHIRE2,
	aHATTER,
	aBILSYBOSS,
	aSKELETON,
	aDOGGYDOGG,
	aNORMTURTLE,
	aHELMUT,
//	aLEAF,
	aDEMON,
//	aFIRE,
//	aLAVA,
	aDRAGONFLY,
	aMONKEY,
	aFATCHK,
	aFENCER,
	aFISH,
//	aMOTH,
//	aSTEAM,
	aROCK,
//	aPINLEFTPADDLE,
//	aPINRIGHTPADDLE,
//	aPIN500BUMP,
//	aPINCARROTBUMP,
	aQUEENBOSS,
	aFLOATSUCKER,
//	aBRIDGE				153
	aFLOATLIZARD,
	aSTANDMONKEY,
	aRAVEN,
	aTUBETURTLE,
//	aGEMRING,
//	aROTSMALLTREE,
	aUTERUS,
	aCRAB,
	aWITCH,
	aROCKTURT,
	aBUBBA,
	aDEVILDEVAN,
	aDEVANROBOT,
	aROBOT,
	//aCARROTUSPOLE,
//	aPSYCHPOLE,
//	aDIAMONDUSPOLE,
//	aSUCKERTUBE,
	//aWATERLEVEL,
	aFRUITPLATFORM,
	aBOLLPLATFORM,
	aGRASSPLATFORM,
	aPINKPLATFORM,
	aSONICPLATFORM,
	aSPIKEPLATFORM,
	aSPIKEBOLL,
	aEVA,
	aMORPHFROG,
	//aSPIKEBOLL3D,
	aSPRINGCORD,
	aBEES,
	//aCOPTER,
	aLASERSHIELD,
	aSTOPWATCH,
//	aJUNGLEPOLE
//	aBIGROCK
//	aBIGBOX
	aSONICBOSS,
	aBUTTERFLY,
	aBEEBOY,
	aTWEEDLEBOSS,
	aAIRBOARD,
	aLASTLOADOBJ,
};	//EvilObjects()







//forward declaration
void SetupEnemies2(void);

// Extremely long functions take extremely long to compile when global optimizations are on

void SetupBullets(void)
{
	Obullets *obj;
	Omonster *mons;

//BULLET1
	obj=(Obullets *)(&loadobj[aPLAYERBULLET1]);
	obj->xspeed=6*65536;
	//obj->yspeed=0;
	obj->xacc=8192;
	obj->force=1;
	obj->ProcPtr=&cBULLET1;
	obj->objtype=oPLAYERBULLET;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
	obj->curanim=AnimBase[mAMMO]+mAMMO_GUN1;
	obj->upanim=AnimBase[mAMMO]+mAMMO_GUN1U;
	obj->type=bullNORMAL;
	obj->lighttype=2;
	obj->var4=1;
	obj->lifetime=AISPEED/2;

//BULLET2
	obj=(Obullets *)(&loadobj[aPLAYERBULLET2]);
	obj->xspeed=5*65536;
	obj->yspeed=65536;
	obj->xacc=16384;
	obj->yacc=6000;
	obj->force=1;
	obj->ProcPtr=&cBOUNCE;
	obj->objtype=oPLAYERBULLET;
	obj->killanim=AnimBase[mAMMO]+mAMMO_BOOM3;
	obj->curanim=AnimBase[mAMMO]+mAMMO_GUN2;
	obj->upanim=AnimBase[mAMMO]+mAMMO_GUN2;
	//obj->type=bullNORMAL;
	obj->lighttype=2;
	obj->var4=2;
	obj->lifetime=(AISPEED*3)/2;

//BULLET3 : FREEZER
	obj=(Obullets *)(&loadobj[aPLAYERBULLET3]);
	obj->xspeed=6*65536;
	obj->xacc=8192;
	obj->force=2;
	obj->ProcPtr=&cBULLET1;
	obj->freeze=200;	//set this to length of freeze!!!
	obj->objtype=oPLAYERBULLET;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS3;
	obj->curanim=AnimBase[mAMMO]+mAMMO_GUN3;
	obj->upanim=AnimBase[mAMMO]+mAMMO_GUN3U;
	//obj->type=bullNORMAL;
	obj->lighttype=2;
	obj->var4=3;
	obj->lifetime=AISPEED/2;


//BULLET4
	obj=(Obullets *)(&loadobj[aPLAYERBULLET4]);
	obj->xspeed=2*65536;
	obj->xacc=16384;
	obj->yacc=256;
	obj->force=2;
	obj->ProcPtr=&cSEEKINGROCKET;
	obj->objtype=oPLAYERBULLET;
	obj->killanim=AnimBase[mAMMO]+mAMMO_BOOM4;
	obj->curanim=AnimBase[mAMMO]+mAMMO_GUN4NR;
	obj->upanim=AnimBase[mAMMO]+mAMMO_GUN4NU;
	//obj->type=bullNORMAL;
	obj->lighttype=2;
	obj->var4=4;
	obj->lifetime=2*AISPEED;

//BULLET5
	obj=(Obullets *)(&loadobj[aPLAYERBULLET5]);
	obj->xspeed=65536;
	//obj->yspeed=0;
	obj->xacc=4096*3;
	obj->ProcPtr=&rocket1;
	obj->force=2;
	obj->objtype=oPLAYERBULLET;
	obj->killanim=AnimBase[mAMMO]+mAMMO_BOOM2;
	obj->curanim=AnimBase[mAMMO]+mAMMO_GUN5;
	obj->upanim=AnimBase[mAMMO]+mAMMO_GUN5U;
	//obj->type=bullNORMAL;
	obj->lighttype=2;
	obj->var4=5;
	obj->lifetime=40;


//BULLET6-flamer
	obj=(Obullets *)(&loadobj[aPLAYERBULLET6]);
	obj->xspeed=4*65536;
	obj->xacc=8192;
	obj->ProcPtr=&flamebullet;
	obj->force=1;
	obj->objtype=oPLAYERBULLET;
	//obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS2;
	obj->curanim=AnimBase[mAMMO]+mAMMO_GUN6;
	obj->upanim=obj->curanim;
	obj->type=bullFLAMER;
	obj->lighttype=0;
	obj->var4=6;
	obj->lifetime=AISPEED/2;

//BULLET8
	obj=(Obullets *)(&loadobj[aPLAYERBULLET8]);
	obj->xspeed=2*65536;
	//obj->yspeed=0;
	obj->xacc=8192;
	obj->force=2;
	obj->ProcPtr=&cBULLET1;
	obj->objtype=oPLAYERBULLET;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
	obj->curanim=AnimBase[mAMMO]+mAMMO_GUN8;
	obj->upanim=AnimBase[mAMMO]+mAMMO_GUN8U;
	//obj->type=bullNORMAL;
	obj->lighttype=2;
	obj->var4=8;
	obj->lifetime=AISPEED/2;

//BULLET9
	obj=(Obullets *)(&loadobj[aPLAYERBULLET9]);
	obj->ProcPtr=&cBLAMMER;
	obj->curanim=obj->upanim=AnimBase[mAMMO]+mAMMO_XPLOS2;	//collision! whatever
	obj->phase=0;
	obj->objtype=oPLAYERBULLET;
	obj->force=1;
	obj->type=bullFLAMER;
	obj->lighttype=2;
	obj->xspeed=4*65536;
	obj->yspeed=0;
	obj->lifetime=AISPEED/2;

/*
	obj->xspeed=2*65536;
	//obj->yspeed=0;
	obj->xacc=8192;
	obj->force=2;
	obj->code=&cBULLET1;
	obj->objtype=oPLAYERBULLET;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
	obj->curanim=AnimBase[mAMMO]+mAMMO_GUN9;
	obj->upanim=AnimBase[mAMMO]+mAMMO_GUN9;
	//obj->type=bullNORMAL;
	obj->lighttype=2;
	obj->var4=9;
*/
	
	
//PBULLET1
	obj=(Obullets *)(&loadobj[aPLAYERBULLETP1]);
	obj->xspeed=7*65536;
	//obj->yspeed=0;
	obj->xacc=4096*3;
	obj->force=2;
	obj->ProcPtr=&cBULLET1;
	obj->objtype=oPLAYERBULLET;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
	obj->curanim=AnimBase[mAMMO]+mAMMO_GUN1P;
	obj->upanim=AnimBase[mAMMO]+mAMMO_GUN1UP;
	obj->type=bullPOWER;
	obj->lighttype=1;
	obj->var4=1;
	obj->lifetime=30;

//PBULLET2
	obj=(Obullets *)(&loadobj[aPLAYERBULLETP2]);
	obj->xspeed=5*65536;
	obj->yspeed=65536;
	obj->xacc=16384;
	obj->yacc=6000;
	obj->force=2;
	obj->ProcPtr=&cBOUNCE2;
	obj->objtype=oPLAYERBULLET;
	obj->killanim=AnimBase[mAMMO]+mAMMO_BOOM3;
	obj->curanim=AnimBase[mAMMO]+mAMMO_GUN2P;
	obj->upanim=AnimBase[mAMMO]+mAMMO_GUN2P;
	obj->type=bullPOWER;
	obj->lighttype=2;
	obj->var4=2;
	obj->lifetime=(AISPEED*3)/2;


//PBULLET3 : FREEZER
	obj=(Obullets *)(&loadobj[aPLAYERBULLETP3]);
	obj->xspeed=7*65536;
	obj->xacc=4096*3;
	obj->force=2;
	obj->ProcPtr=&cSINEBULLET1;
	obj->freeze=254;	//set this to length of freeze!!!
	obj->objtype=oPLAYERBULLET;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS3;
	obj->curanim=AnimBase[mAMMO]+mAMMO_GUN3P;
	obj->upanim=AnimBase[mAMMO]+mAMMO_GUN3UP;
	obj->type=bullPOWER;
	obj->lighttype=2;
	obj->var4=3;
	obj->lifetime=30;


//PBULLET4
	obj=(Obullets *)(&loadobj[aPLAYERBULLETP4]);
	obj->xspeed=32768*3;
	obj->xacc=16384;
	obj->yacc=256;
	obj->force=3;
	obj->ProcPtr=&cSEEKINGROCKET;
	obj->objtype=oPLAYERBULLET;
	obj->killanim=AnimBase[mAMMO]+mAMMO_BOOM4;
	obj->curanim=AnimBase[mAMMO]+mAMMO_GUN4PR;
	obj->upanim=AnimBase[mAMMO]+mAMMO_GUN4PU;
	obj->type=bullPOWER;
	obj->lighttype=2;
	obj->var4=4;
	obj->lifetime=2*AISPEED;


//PBULLET5
	obj=(Obullets *)(&loadobj[aPLAYERBULLETP5]);
	obj->xspeed=32768*3;
	//obj->yspeed=0;
	obj->xacc=4096*3;
	obj->ProcPtr=&rocket1;
	obj->force=2;
	obj->objtype=oPLAYERBULLET;
	obj->killanim=AnimBase[mAMMO]+mAMMO_BOOM2;
	obj->curanim=AnimBase[mAMMO]+mAMMO_GUN5P;
	obj->upanim=AnimBase[mAMMO]+mAMMO_GUN5PU;
	obj->type=bullPOWER;
	obj->lighttype=2;
	obj->var4=5;
	obj->lifetime=AISPEED/2;


//PBULLET6-flamer
	obj=(Obullets *)(&loadobj[aPLAYERBULLETP6]);
	obj->xspeed=4*65536;
	obj->xacc=8192;
	obj->ProcPtr=&flamebullet;
	obj->force=1; //too strong already
	obj->objtype=oPLAYERBULLET;
	//obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS2;
	obj->curanim=AnimBase[mAMMO]+mAMMO_GUN6P;
	obj->upanim=AnimBase[mAMMO]+mAMMO_GUN6P;
	obj->type=bullFLAMER+bullPOWER;
	obj->lighttype=0;
	obj->var4=6;
	obj->lifetime=AISPEED/2;


//PBULLET8
	obj=(Obullets *)(&loadobj[aPLAYERBULLETP8]);
	obj->xspeed=4*65536;
	//obj->yspeed=0;
	obj->xacc=4096*4;
	obj->force=3;
	obj->ProcPtr=&cBULLET1;
	obj->objtype=oPLAYERBULLET;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
	obj->curanim=AnimBase[mAMMO]+mAMMO_GUN8P;
	obj->upanim=AnimBase[mAMMO]+mAMMO_GUN8UP;
	obj->type=bullPOWER;
	obj->lighttype=1;
	obj->var4=8;
	obj->lifetime=AISPEED/2;

//PBULLET9
	obj=(Obullets *)(&loadobj[aPLAYERBULLETP9]);

	obj->ProcPtr=&cBLAMMER;
	obj->curanim=obj->upanim=AnimBase[mAMMO]+mAMMO_XPLOS2;	//collision! whatever
	obj->phase=0;
	obj->objtype=oPLAYERBULLET;
	obj->force=1;
	obj->type=bullFLAMER;
	obj->lighttype=2;
	obj->xspeed=4*65536;
	obj->yspeed=0;
	obj->lifetime=AISPEED/2;

//PCRAZYBULLET 1- fire
	obj=(Obullets *)(&loadobj[aPLAYERBULLETC1]);
	obj->xspeed=2*65536;
	obj->xacc=8192;
	obj->ProcPtr=&cBULLET1;
	obj->force=3;
	obj->objtype=oPLAYERBULLET;
	//obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS2;
	obj->curanim=AnimBase[mAMMO]+mAMMO_FIRESHOT;
	obj->type=bullFLAMER;
	obj->lighttype=2;
	obj->var4=6;
	obj->lifetime=(AISPEED*2)/3;


//PCRAZYBULLET 2 -bubble 
	obj=(Obullets *)(&loadobj[aPLAYERBULLETC2]);
	obj->xspeed=2*65536;
	obj->xacc=8192;
	obj->ProcPtr=&cBUBBLYSPRAY;
	obj->force=1;
	obj->objtype=oPLAYERBULLET;
	//obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS2;
	obj->curanim=AnimBase[mAMMO]+mAMMO_BUB;
	obj->type=bullPOWER;
	obj->lighttype=1;
	obj->var4=1;
	obj->lifetime=(AISPEED*2)/3;


//PCRAZYBULLET 3 -electricity
	obj=(Obullets *)(&loadobj[aPLAYERBULLETC3]);
	obj->xspeed=2*65536;
	obj->xacc=8192;
	obj->ProcPtr=&cBULLET1;
	obj->force=3;
	obj->objtype=oPLAYERBULLET;
	//obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS2;
	obj->curanim=AnimBase[mAMMO]+mAMMO_ELECSHOT;
	obj->type=bullPOWER;
	obj->lighttype=1;
	obj->var4=1;
	obj->lifetime=(AISPEED*2)/3;

//LASER
	obj=(Obullets *)(&loadobj[aPLAYERLASER]);
	obj->xspeed=16*65536; //for moving objects around!
	obj->xacc=4*65536;
	obj->ProcPtr=&cLASER;
	obj->force=8;
	obj->objtype=oPLAYERBULLET;
	//obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS2;
	//obj->curanim=AnimBase[mAMMO]+mAMMO_ELECSHOT;
	obj->type=bullPOWER+bullLASER+bullFLAMER;
	obj->lighttype=6;
	obj->light=127;
	obj->var4=8; //goes through anything


//TNNT
	obj=(Obullets *)(&loadobj[aTNT]);
	obj->ProcPtr=&cTNT;
	obj->energy=1; //minimum, always gets killed
	obj->objtype=oSPECIAL;
	obj->curanim=AnimBase[mAMMO]+mAMMO_GUN7ITEM;
	obj->killanim=AnimBase[mAMMO]+mAMMO_TNTEXPL;
	//obj->type=bullPOWER;
	obj->lighttype=1;
	obj->var4=7;
	obj->lifetime=255;



//BOMB
	obj=(Obullets *)(&loadobj[aBOMB]);
	obj->ProcPtr=&bomb;
	obj->objtype=oSPECIAL;
	obj->direction=1;
	//obj->energy=0;
	obj->curanim=AnimBase[mAMMO]+mAMMO_BOMB;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS2;
	obj->lighttype=2;
	obj->lifetime=AISPEED*3/2;


//BANANA
	obj=(Obullets *)(&loadobj[aBANANA]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_BANANA;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;
	obj->lifetime=AISPEED/2;



///////////////////////////////////

//GENERAL SHARD
	mons=(Omonster *)(&loadobj[aSHARD]);
	mons->ProcPtr=&cRANDPART;
	mons->objtype=oPARTICLE;

//GENERAL SHARD2
	mons=(Omonster *)(&loadobj[aBOUNCEONCE]);
	mons->ProcPtr=&cRANDPART2;
	mons->objtype=oPARTICLE;


//GENERAL SMOKE/EXPLOSION PARTICLE
	mons=(Omonster *)(&loadobj[aEXPLOSION]);
	mons->ProcPtr=&cEXPLOSION;
	mons->objtype=oPARTICLE;
	mons->lighttype=1;
//	mons->curanim=AnimBase[mAMMO]+mAMMO_POOF1;


} //SetupBullets()



/*
blah points costs

10000 devil devan|devil|devan

2000 LASER SHIELD|laser|shield
*/
void SetupEnemies(void)
{
	Omonster *obj;
/*
//AMMOUP 1
	obj=(Omonster *)(&loadobj[aGUN1AMMO3]);
	obj->code=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->loadanim=mPICKUPS;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_UPGUN;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
*/
//AMMOUP 2
	obj=(Omonster *)(&loadobj[aGUN2AMMO3]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mAMMO]+mAMMO_GUN2ITEM;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->var4=1;
	obj->points=100;

//AMMOUP 3
	obj=(Omonster *)(&loadobj[aGUN3AMMO3]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mAMMO]+mAMMO_GUN3ITEM;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->var4=2;
	obj->points=100;


//AMMOUP 4
	obj=(Omonster *)(&loadobj[aGUN4AMMO3]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mAMMO]+mAMMO_GUN4ITEM;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->var4=3;
	obj->points=100;


//AMMOUP 5
	obj=(Omonster *)(&loadobj[aGUN5AMMO3]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mAMMO]+mAMMO_GUN5ITEM;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->var4=4;
	obj->points=100;


//AMMOUP 6
	obj=(Omonster *)(&loadobj[aGUN6AMMO3]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	//obj->direction=0;
	obj->curanim=AnimBase[mAMMO]+mAMMO_GUN6ITEM;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->var4=5;
	obj->points=100;


//AMMOUP 7
	obj=(Omonster *)(&loadobj[aGUN7AMMO3]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mAMMO]+mAMMO_GUN7ITEM; //TNT!
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->var4=6;
	obj->points=100;


//AMMOUP 8
	obj=(Omonster *)(&loadobj[aGUN8AMMO3]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mAMMO]+mAMMO_GUN8ITEM;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->var4=7;
	obj->points=100;


//AMMOUP 9
	obj=(Omonster *)(&loadobj[aGUN9AMMO3]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mAMMO]+mAMMO_GUN9ITEM;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->var4=8;
	obj->points=100;

//AMMOUP15 2
	obj=(Omonster *)(&loadobj[aGUN2AMMO15]);
	obj->ProcPtr=&ammomonitor;
	obj->objtype=oSPECIAL;
	obj->direction=1;
	obj->energy=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_MONITAM2;
	obj->killanim=AnimBase[mAMMO]+mAMMO_POOF1;
	obj->var4=1;
	obj->var3=aGUN2AMMO3;
	obj->points=300;

//AMMOUP15 3
	obj=(Omonster *)(&loadobj[aGUN3AMMO15]);
	obj->ProcPtr=&ammomonitor;
	obj->objtype=oSPECIAL;
	obj->direction=1;
	obj->energy=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_MONITAM3;
	obj->killanim=AnimBase[mAMMO]+mAMMO_POOF1;
	obj->var4=2;
	obj->var3=aGUN3AMMO3;
	obj->points=300;

//AMMOUP15 4
	obj=(Omonster *)(&loadobj[aGUN4AMMO15]);
	obj->ProcPtr=&ammomonitor;
	obj->objtype=oSPECIAL;
	obj->direction=1;
	obj->energy=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_MONITAM4;
	obj->killanim=AnimBase[mAMMO]+mAMMO_POOF1;
	obj->var4=3;
	obj->var3=aGUN4AMMO3;
	obj->points=300;

//AMMOUP15 5
	obj=(Omonster *)(&loadobj[aGUN5AMMO15]);
	obj->ProcPtr=&ammomonitor;
	obj->objtype=oSPECIAL;
	obj->direction=1;
	obj->energy=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_MONITAM5;
	obj->killanim=AnimBase[mAMMO]+mAMMO_POOF1;
	obj->var4=4;
	obj->var3=aGUN5AMMO3;
	obj->points=300;

//AMMOUP15 6
	obj=(Omonster *)(&loadobj[aGUN6AMMO15]);
	obj->ProcPtr=&ammomonitor;
	obj->objtype=oSPECIAL;
	obj->direction=1;
	obj->energy=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_MONITAM6;
	obj->killanim=AnimBase[mAMMO]+mAMMO_POOF1;
	obj->var4=5;
	obj->var3=aGUN6AMMO3;
	obj->points=300;


//AMMO POWERUP
	obj=(Omonster *)(&loadobj[aGUNFASTFIRE]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_FASTFIRE;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=200;

//BLASTER POWERUP
	obj=(Omonster *)(&loadobj[aGUN1POWER]);
	obj->ProcPtr=&monitor;
	obj->objtype=oSPECIAL;
	obj->direction=1;
	obj->energy=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_MONITPW1;
	obj->killanim=AnimBase[mAMMO]+mAMMO_POOF1;
	//obj->var4=0;
	obj->points=1000;

//BOUNCY POWERUP
	obj=(Omonster *)(&loadobj[aGUN2POWER]);
	obj->ProcPtr=&monitor;
	obj->objtype=oSPECIAL;
	obj->direction=1;
	obj->energy=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_MONITPW2;
	obj->killanim=AnimBase[mAMMO]+mAMMO_POOF1;
	obj->var4=1;
	obj->points=1000;

//FREEZE POWERUP
	obj=(Omonster *)(&loadobj[aGUN3POWER]);
	obj->ProcPtr=&monitor;
	obj->objtype=oSPECIAL;
	obj->direction=1;
	obj->energy=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_MONITPW3;
	obj->killanim=AnimBase[mAMMO]+mAMMO_POOF1;
	obj->var4=2;
	obj->points=1000;

//SEEK POWERUP
	obj=(Omonster *)(&loadobj[aGUN4POWER]);
	obj->ProcPtr=&monitor;
	obj->objtype=oSPECIAL;
	obj->direction=1;
	obj->energy=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_MONITPW4;
	obj->killanim=AnimBase[mAMMO]+mAMMO_POOF1;
	obj->var4=3;
	obj->points=1000;

//RF POWERUP
	obj=(Omonster *)(&loadobj[aGUN5POWER]);
	obj->ProcPtr=&monitor;
	obj->objtype=oSPECIAL;
	obj->direction=1;
	obj->energy=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_MONITPW5;
	obj->killanim=AnimBase[mAMMO]+mAMMO_POOF1;
	obj->var4=4;
	obj->points=1000;

//TOASTER POWERUP
	obj=(Omonster *)(&loadobj[aGUN6POWER]);
	obj->ProcPtr=&monitor;
	obj->objtype=oSPECIAL;
	obj->direction=1;
	obj->energy=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_MONITPW6;
	obj->killanim=AnimBase[mAMMO]+mAMMO_POOF1;
	obj->var4=5;
	obj->points=1000;


//TNT POWERUP
	obj=(Omonster *)(&loadobj[aTNTPOWER]);
	obj->ProcPtr=&monitor;
	obj->objtype=oSPECIAL;
	obj->direction=1;
	obj->energy=1;
//	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_MONITPW7;
	obj->killanim=AnimBase[mAMMO]+mAMMO_POOF1;
	obj->var4=6;
	obj->points=1000;


//GUN8 POWERUP
	obj=(Omonster *)(&loadobj[aGUN8POWER]);
	obj->ProcPtr=&monitor;
	obj->objtype=oSPECIAL;
	obj->direction=1;
	obj->energy=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_MONITPW8;
	obj->killanim=AnimBase[mAMMO]+mAMMO_POOF1;
	obj->var4=7;
	obj->points=1000;

//GUN9 POWERUP
	obj=(Omonster *)(&loadobj[aGUN9POWER]);
	obj->ProcPtr=&monitor;
	obj->objtype=oSPECIAL;
	obj->direction=1;
	obj->energy=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_MONITPW9;
	obj->killanim=AnimBase[mAMMO]+mAMMO_POOF1;
	obj->var4=8;
	obj->points=1000;


//TEMPORARY GEM RED +1
	obj=(Omonster *)(&loadobj[aREDGEMTEMP]);
	obj->ProcPtr=&timedfloating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->nohit=1;
	//obj->energy=UNLIMITED;
	obj->counter=8*AISPEED;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_GEMSMALL;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->var1=1;
	obj->points=100;


//GEM RED +1
	obj=(Omonster *)(&loadobj[aREDGEM]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->nohit=1;
	//obj->energy=UNLIMITED;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_GEMSMALL;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->var1=1;
	obj->points=100;

//GEM GREEN +1
	obj=(Omonster *)(&loadobj[aGREENGEM]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->nohit=1;
	//obj->energy=UNLIMITED;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_GEMSMALL;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->var1=2;
	obj->points=500;

//GEM BLUE +1
	obj=(Omonster *)(&loadobj[aBLUEGEM]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->nohit=1;
	//obj->energy=UNLIMITED;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_GEMSMALL;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->var1=3;
	obj->points=1000;

//GEM PURPLE +1
	obj=(Omonster *)(&loadobj[aPURPLEGEM]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->nohit=1;
	//obj->energy=UNLIMITED;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_GEMSMALL;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->var1=4;
	obj->points=100;

//SUPER RED GEM +1
	obj=(Omonster *)(&loadobj[aSUPERREDGEM]);
	obj->ProcPtr=&biggem;
	obj->objtype=oSPECIAL;
	obj->direction=1;
	obj->energy=2;
	obj->var1=1;
	obj->nohit=8;	//nofreeze
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_GEMLARGE;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;


//BIRDCAGE
	obj=(Omonster *)(&loadobj[aBIRDCAGE]);
	obj->ProcPtr=&birdcage;
	obj->energy=127;
	obj->objtype=oSPECIAL+oTRIGGERTNT;
	obj->direction=1;
	obj->loadanim=mBIRD; //special: depends on event data
	obj->var1=-1;
	obj->lighttype=2;
	obj->points=300;

//FLIP THE BIRD
	obj=(Omonster *)(&loadobj[aBIRD]);
	obj->ProcPtr=&birdy;
	obj->nohit=1+8+16;
	obj->energy=127;
	//obj->energy=UNLIMITED; //!
	obj->objtype=oSPECIAL;
	obj->direction=1;
	obj->loadanim=mBIRD;
	obj->killanim=AnimBase[mBIRD]+mBIRD_DIE;
	obj->var1=-1;
	obj->lighttype=2;

//BUBBLE
	obj=(Omonster *)(&loadobj[aBUBBLE]);
	obj->ProcPtr=&cBUBBLE;
	obj->nohit=1+8;
	obj->energy=1;
	obj->objtype=oSPECIAL;
	obj->curanim=AnimBase[mCOMMON]+mCOMMON_BUBBLES;
	//obj->phase=0;

//BUBBLER
	obj=(Omonster *)(&loadobj[aBUBBLER]);
	obj->ProcPtr=&cBUBBLER;
	obj->nohit=1+8;
	obj->energy=1;
	obj->objtype=oSPECIAL;
	obj->curanim=AnimBase[mCOMMON]+mCOMMON_BUBBLES;


//WOODEN BOX
	obj=(Omonster *)(&loadobj[aGEMCRATE]);
	obj->ProcPtr=&boxx;
	obj->objtype=oSPECIAL+oTRIGGERTNT;
	obj->direction=1;
	obj->energy=2;
	obj->special=-1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_BOX;
	obj->points=100;
//	obj->killanim=0;	//AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;

//WOODEN BOX
	obj=(Omonster *)(&loadobj[aGUNCRATE]);
	obj->ProcPtr=&boxx;
	obj->objtype=oSPECIAL+oTRIGGERTNT;
	obj->direction=1;
	obj->energy=2;
	obj->special=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_BOX;
//	obj->killanim=0;	//AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=100;

//WOODEN BOX
	obj=(Omonster *)(&loadobj[aCARROTCRATE]);
	obj->ProcPtr=&boxx;
	obj->objtype=oSPECIAL+oTRIGGERTNT;
	obj->direction=1;
	obj->energy=2;
	obj->special=2;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_BOX;
//	obj->killanim=0;	//AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=100;

//WOODEN BOX
	obj=(Omonster *)(&loadobj[a1UPCRATE]);
	obj->ProcPtr=&boxx;
	obj->objtype=oSPECIAL+oTRIGGERTNT;
	obj->direction=1;
	obj->energy=2;
	obj->special=3;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_BOX;
//	obj->killanim=0;	//AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=100;



//GUN BARREL
	obj=(Omonster *)(&loadobj[aGUNBARREL]);
	obj->ProcPtr=&boxx;
	obj->objtype=oSPECIAL+oTRIGGERTNT;
	obj->direction=1;
	obj->nohit=3;
	//obj->energy=UNLIMITED;
	obj->special=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_BARREL1;
//	obj->killanim=0;	//AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=100;

//GEM BARREL
	obj=(Omonster *)(&loadobj[aGEMBARREL]);
	obj->ProcPtr=&boxx;
	obj->objtype=oSPECIAL+oTRIGGERTNT;
	obj->direction=1;
	obj->nohit=3;
	//obj->energy=UNLIMITED;
	obj->special=-1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_BARREL1;
//	obj->killanim=0;	//AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=100;

//CARROT BARREL
	obj=(Omonster *)(&loadobj[aCARROTBARREL]);
	obj->ProcPtr=&boxx;
	obj->objtype=oSPECIAL+oTRIGGERTNT;
	obj->direction=1;
	obj->nohit=3;
	//obj->energy=UNLIMITED;
	obj->special=2;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_BARREL1;
//	obj->killanim=0;	//AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=100;

//1UP BARREL
	obj=(Omonster *)(&loadobj[a1UPBARREL]);
	obj->ProcPtr=&boxx;
	obj->objtype=oSPECIAL+oTRIGGERTNT;
	obj->direction=1;
	obj->nohit=3;
	//obj->energy=UNLIMITED;
	obj->special=3;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_BARREL1;
//	obj->killanim=0;	//AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=100;


//TRIGGER CRATE
	obj=(Omonster *)(&loadobj[aTRIGGERCRATE]);
	obj->ProcPtr=&boxx;
	obj->objtype=oSPECIAL+oTRIGGERTNT;
	obj->direction=1;
	obj->nohit=3;
	//	obj->energy=UNLIMITED;
	obj->special=6;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_METALCR;
	obj->points=250;
//	obj->killanim=0;	//AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;


//SPRING CRATE
	obj=(Omonster *)(&loadobj[aSPRINGCRATE]);
	obj->ProcPtr=&boxx;
	obj->objtype=oSPECIAL+oTRIGGERTNT;
	obj->direction=1;
	//obj->nohit=2;
	obj->energy=5;
	obj->special=4; //spring crate (see boxx)
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_BOX;
//	obj->killanim=0;	//AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=100;


//BOMB CRATE
	obj=(Omonster *)(&loadobj[aBOMBCRATE]);
	obj->ProcPtr=&boxx;
	obj->objtype=oSPECIAL+oTRIGGERTNT;
	obj->direction=1;
	//obj->nohit=2;
	obj->energy=5;
	obj->special=5;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_BOX;
//	obj->killanim=0;	//AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=100;

//APPLE
	obj=(Omonster *)(&loadobj[aAPPLE]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_APPLE;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;

//PEAR
	obj=(Omonster *)(&loadobj[aPEAR]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_PEAR;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;
//ORANGE
	obj=(Omonster *)(&loadobj[aORANGE]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_ORANGE;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;
//CHERRY
	obj=(Omonster *)(&loadobj[aCHERRY]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_CHERRY;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;
//PRETZEL
	obj=(Omonster *)(&loadobj[aPRETZEL]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_PRETZEL;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;
//STRAWBERRY
	obj=(Omonster *)(&loadobj[aSTRAWBERRY]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_SBERRY;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;
//LEMON
	obj=(Omonster *)(&loadobj[aLEMON]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_LEMON;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;
//LIME
	obj=(Omonster *)(&loadobj[aLIME]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_LIME;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;
//THING
	obj=(Omonster *)(&loadobj[aTHING]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_THING;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;
//WMELON
	obj=(Omonster *)(&loadobj[aWMELON]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_WMELON;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;
//PEACH
	obj=(Omonster *)(&loadobj[aPEACH]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_PEACH;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;
//GRAPES
	obj=(Omonster *)(&loadobj[aGRAPES]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_GRAPES;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;
//LETTUCE
	obj=(Omonster *)(&loadobj[aLETTUCE]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_LETTUCE;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;
//EGGPLANT
	obj=(Omonster *)(&loadobj[aEGGPLANT]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_EGGPLANT;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;
//CUCUMB
	obj=(Omonster *)(&loadobj[aCUCUMB]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_CUCUMB;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;
//COKE
	obj=(Omonster *)(&loadobj[aCOKE]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_COKE;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;
//PEPSI
	obj=(Omonster *)(&loadobj[aPEPSI]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_PEPSI;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;
//MILK
	obj=(Omonster *)(&loadobj[aMILK]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_MILK;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;
//PIE
	obj=(Omonster *)(&loadobj[aPIE]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_PIE;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;
//CAKE
	obj=(Omonster *)(&loadobj[aCAKE]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_CAKE;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;
//DONUT
	obj=(Omonster *)(&loadobj[aDONUT]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_DONUT;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;
//CUPCAKE
	obj=(Omonster *)(&loadobj[aCUPCAKE]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_CUPCAKE;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;
//CHIPS
	obj=(Omonster *)(&loadobj[aCHIPS]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_CHIPS;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;
//CANDY1
	obj=(Omonster *)(&loadobj[aCANDY1]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_CANDY1;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;
//CHOCBAR
	obj=(Omonster *)(&loadobj[aCHOCBAR]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_CHOCBAR;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;
//ICECREAM
	obj=(Omonster *)(&loadobj[aICECREAM]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_ICECREAM;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;
//BURGER
	obj=(Omonster *)(&loadobj[aBURGER]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_BURGER;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;
//PIZZA
	obj=(Omonster *)(&loadobj[aPIZZA]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_PIZZA;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;
//FRIES
	obj=(Omonster *)(&loadobj[aFRIES]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_FRIES;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;
//CHICKLEG
	obj=(Omonster *)(&loadobj[aCHICKLEG]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_CHICKLEG;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;
//SANDWICH
	obj=(Omonster *)(&loadobj[aSANDWICH]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_SANDWICH;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;
//TACOBELL
	obj=(Omonster *)(&loadobj[aTACOBELL]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_TACOBELL;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;
//WEENIE
	obj=(Omonster *)(&loadobj[aWEENIE]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_WEENIE;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;
//HAM
	obj=(Omonster *)(&loadobj[aHAM]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_HAM;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;
//CHEESE
	obj=(Omonster *)(&loadobj[aCHEESE]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_CHEESE;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=50;



//CARROT
	obj=(Omonster *)(&loadobj[aENERGYUP]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_CRRT;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=200;

//SILVER COIN
	obj=(Omonster *)(&loadobj[aSILVERCOIN]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_SILVCOIN;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
//	obj->var1=0; //gem=0
	obj->points=500;

//GOLD COIN
	obj=(Omonster *)(&loadobj[aGOLDCOIN]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_GOLDCOIN;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
//	obj->var1=0; //gem=0
	obj->points=1000;

//CARROT FULL
	obj=(Omonster *)(&loadobj[aFULLENERGY]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_SCRRT;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=500;

//SHIELD FIRE
	obj=(Omonster *)(&loadobj[aFIRESHIELD]);
	obj->ProcPtr=&monitor;
	obj->objtype=oSPECIAL+oTRIGGERTNT;
	obj->direction=1;
	obj->energy=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_FIRMONIT;
	obj->killanim=AnimBase[mAMMO]+mAMMO_POOF1;
	obj->points=1000;
//SHIELD WATER
	obj=(Omonster *)(&loadobj[aWATERSHIELD]);
	obj->ProcPtr=&monitor;
	obj->objtype=oSPECIAL+oTRIGGERTNT;
	obj->direction=1;
	obj->energy=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_BUBMONIT;
	obj->killanim=AnimBase[mAMMO]+mAMMO_POOF1;
	obj->points=1000;
//SHIELD LIGHTNING
	obj=(Omonster *)(&loadobj[aLIGHTSHIELD]);
	obj->ProcPtr=&monitor;
	obj->objtype=oSPECIAL+oTRIGGERTNT;
	obj->direction=1;
	obj->energy=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_LITMONIT;
	obj->killanim=AnimBase[mAMMO]+mAMMO_POOF1;
	obj->points=1000;

//POWER CARROT : INVINCIBILITY
	obj=(Omonster *)(&loadobj[aINVINCIBILITY]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP+oTRIGGERTNT;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_PCRRT;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=500;

//EXTRA TIME
	obj=(Omonster *)(&loadobj[aEXTRATIME]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP+oTRIGGERTNT;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_STPWCH;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;

//FREEZE ENEMIES
	obj=(Omonster *)(&loadobj[aFREEZER]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP+oTRIGGERTNT;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_HRGLSS;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;

//FASTFEET
	obj=(Omonster *)(&loadobj[aFASTFEET]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP+oTRIGGERTNT;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_FSFEET;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;

//EXTRA LIFE
	obj=(Omonster *)(&loadobj[aEXTRALIFE]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP+oTRIGGERTNT;
	//obj->direction=0;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_1UP;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->lighttype=7; //blobs!
	obj->points=2000;

//EOL POST
	obj=(Omonster *)(&loadobj[aENDOFLEVELPOST]);
	obj->ProcPtr=&eolpost;
	obj->objtype=oSPECIAL;
	//obj->direction=0;
	obj->energy=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_EXITSIGN;
	obj->nohit=8;
	//obj->killanim=AnimBase[mAMMO]+mAMMO_POOF1;

//SAVE POST
	obj=(Omonster *)(&loadobj[aSAVEPOST]);
	obj->ProcPtr=&savepost;
	obj->objtype=oSPECIAL;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_CHCKPNT;
	obj->nohit=8;

//BONUS LEVEL POST
	obj=(Omonster *)(&loadobj[aBONUSLEVELPOST]);
	obj->ProcPtr=&bonuspost;
	obj->objtype=oSPECIAL+oTRIGGERTNT;
	obj->direction=1;
	obj->loadanim=mBONUS;
	obj->curanim=AnimBase[mBONUS]+mBONUS_IDLE;
	obj->nohit=8;

//SPRING RED
	obj=(Omonster *)(&loadobj[aREDSPRING]);
	obj->objtype=oSPECIAL;
	obj->yspeed=-16*65536;
	obj->ProcPtr=&spring;
	obj->nohit=1+16;
	//obj->energy=UNLIMITED;
//	obj->loadanim=mSPRING;
	obj->curanim=AnimBase[mSPRING]+mSPRING_RED;

//SPRING GREEN
	obj=(Omonster *)(&loadobj[aGREENSPRING]);
	obj->objtype=oSPECIAL;
	obj->yspeed=-24*65536;
	obj->ProcPtr=&spring;
	obj->nohit=1+16;
	//obj->energy=UNLIMITED;
//	obj->loadanim=mSPRING;
	obj->curanim=AnimBase[mSPRING]+mSPRING_GREEN;

//FROZEN GREEN SPRING
	obj=(Omonster *)(&loadobj[aFROZENGREENSPRING]);
	obj->objtype=oSPECIAL;
	obj->yspeed=-24*65536;
	obj->ProcPtr=&spring;
	obj->nohit=3+16; //hit once! (toggle)
	//obj->energy=UNLIMITED;
	obj->curanim=AnimBase[mSPRING]+mSPRING_GREEN;
	obj->freeze=0xFF; //mucho!
	
//SPRING BLUE
	obj=(Omonster *)(&loadobj[aBLUESPRING]);
	obj->objtype=oSPECIAL;
	obj->yspeed=-32*65536;
	obj->ProcPtr=&spring;
	obj->nohit=1+16;
	//obj->energy=UNLIMITED;
//	obj->loadanim=mSPRING;
	obj->curanim=AnimBase[mSPRING]+mSPRING_BLUE;


//SPRING RED
	obj=(Omonster *)(&loadobj[aHREDSPRING]);
	obj->objtype=oSPECIAL;
	obj->xspeed=16*65536;
	obj->ProcPtr=&spring;
	obj->nohit=1+4+16;
	//obj->energy=UNLIMITED;
//	obj->loadanim=mSPRING;
	obj->curanim=AnimBase[mSPRING]+mSPRING_REDHOR;
//SPRING GREEN
	obj=(Omonster *)(&loadobj[aHGREENSPRING]);
	obj->objtype=oSPECIAL;
	obj->xspeed=24*65536;
	obj->ProcPtr=&spring;
	obj->nohit=1+4+16;
	//obj->energy=UNLIMITED;
//	obj->loadanim=mSPRING;
	obj->curanim=AnimBase[mSPRING]+mSPRING_GREENHOR;
//SPRING BLUE
	obj=(Omonster *)(&loadobj[aHBLUESPRING]);
	obj->objtype=oSPECIAL;
	obj->xspeed=32*65536;
	obj->ProcPtr=&spring;
	obj->nohit=1+4+16;
	//obj->energy=UNLIMITED;
//	obj->loadanim=mSPRING;
	obj->curanim=AnimBase[mSPRING]+mSPRING_BLUEHOR;


//AIRBOARDGENERATOR
	obj=(Omonster *)(&loadobj[aAIRBOARDGENERATOR]);
	obj->ProcPtr=&cAIRBOARDGENERATOR;
	obj->objtype=oSPECIAL;
	obj->nohit=3;
	obj->energy=30;

	
//AIRBOARD
	obj=(Omonster *)(&loadobj[aAIRBOARD]);
	obj->ProcPtr=&cAIRBOARD;
	obj->xspeed=1; //fall!
	obj->objtype=oPICKUP+oTRIGGERTNT;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_GETBOARD;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;


//MORPH TO BIRD!
	obj=(Omonster *)(&loadobj[aBIRDMORPHMONITOR]);
	obj->ProcPtr=&monitor;
	obj->objtype=oSPECIAL+oTRIGGERTNT;
	obj->energy=1;
	obj->direction=1;
	obj->energy=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_MORPHBRD;
	obj->killanim=AnimBase[mAMMO]+mAMMO_POOF1;

//JAZZSPAZ POST
	obj=(Omonster *)(&loadobj[aMORPHMONITOR]);
	obj->ProcPtr=&monitor;
	obj->objtype=oSPECIAL+oTRIGGERTNT;
	obj->direction=1;
	obj->energy=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_MORPHJS;
	obj->killanim=AnimBase[mAMMO]+mAMMO_POOF1;



//FLY CARROT :
	obj=(Omonster *)(&loadobj[aFLYCARROT]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_HCARROT;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->points=300;


//GEM LBRect RED +1
	obj=(Omonster *)(&loadobj[aRECTREDGEM]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->nohit=1;
	//obj->energy=UNLIMITED;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_CRYSMALL;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->var1=1;
	obj->points=100;

//GEM LBRect GREEN +1
	obj=(Omonster *)(&loadobj[aRECTGREENGEM]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->nohit=1;
	//obj->energy=UNLIMITED;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_CRYSMALL;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->var1=2;
	obj->points=500;

//GEM LBRect BLUE +1
	obj=(Omonster *)(&loadobj[aRECTBLUEGEM]);
	obj->ProcPtr=&floating;
	obj->objtype=oPICKUP;
	obj->direction=1;
	obj->nohit=1;
	//obj->energy=UNLIMITED;
	obj->curanim=AnimBase[mPICKUPS]+mPICKUPS_CRYSMALL;
	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;
	obj->var1=3;
	obj->points=1000;


//SWINGVINE
	obj=(Omonster *)(&loadobj[aSWINGVINE]);
	obj->ProcPtr=&cVINE;
	obj->objtype=oSPECIAL;
	obj->direction=1;
	obj->nohit=1;
	//obj->energy=UNLIMITED;
	obj->loadanim=mVINE;
	obj->curanim=AnimBase[mVINE]+mVINE_END;
//	obj->killanim=AnimBase[mPICKUPS]+mPICKUPS_SPRKLE;




//TUFTURT
	obj=(Omonster *)(&loadobj[aTUFTURT]);
	obj->ProcPtr=&walker;
	obj->xspeed=65536;
	obj->energy=4;
	obj->objtype=oNORMAL+oTRIGGERTNT+oENEMY;
	obj->loadanim=mTUFTURT;
	obj->curanim=AnimBase[mTUFTURT]+mTUFTURT_WALK;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS2;
	obj->points=500;


//LABRAT
	obj=(Omonster *)(&loadobj[aLABRAT]);
	obj->objtype=oNORMAL+oTRIGGERTNT+oENEMY;
	obj->ProcPtr=&labrat;
	obj->xspeed=65536;
	obj->energy=1;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
	obj->loadanim=mLABRAT;
	obj->curanim=AnimBase[mLABRAT]+mLABRAT_WALK;
	obj->points=200;

//LIZARD
	obj=(Omonster *)(&loadobj[aLIZARD]);
	obj->objtype=oNORMAL+oTRIGGERTNT+oENEMY;
	obj->ProcPtr=&walker;
	obj->xspeed=65536;
	obj->energy=1;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
	obj->loadanim=mLIZARD;
	obj->curanim=AnimBase[mLIZARD]+mLIZARD_WALK;
	obj->points=100;

//FLOAT LIZARD
	obj=(Omonster *)(&loadobj[aFLOATLIZARD]);
	obj->ProcPtr=&floatlizard;
	obj->xspeed=65536;
	obj->energy=4;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
	obj->loadanim=mLIZARD;
	obj->curanim=AnimBase[mLIZARD]+mLIZARD_FLY;
	obj->objtype=oSPECIAL+oTRIGGERTNT+oENEMY;
	obj->points=200;

//COPTER
	obj=(Omonster *)(&loadobj[aCOPTER]);
	obj->ProcPtr=&copter;
//	obj->xspeed=16384;
	obj->energy=4;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
	obj->loadanim=mLIZARD;
	obj->curanim=AnimBase[mLIZARD]+mLIZARD_JETPACK;
	obj->objtype=oSPECIAL+oTRIGGERTNT;
	obj->nohit=1+8+16;


//SUCKER
	obj=(Omonster *)(&loadobj[aSUCKER]);
	obj->ProcPtr=&sucker;
	obj->xspeed=65536;
	obj->energy=4;
	obj->killanim=AnimBase[mAMMO]+mAMMO_XPLOS1;
	obj->loadanim=mSUCKER;
	obj->curanim=AnimBase[mSUCKER]+mSUCKER_GROUND;
	obj->points=100;
	obj->objtype=oNORMAL+oTRIGGERTNT+oENEMY;


//CATERPILLAR (PSYCH LEV)
	obj=(Omonster *)(&loadobj[aCATERPILLAR]);
	obj->ProcPtr=&caterpillar;
	obj->energy=127;
	//obj->energy=UNLIMITED;
	obj->loadanim=mCATERPIL;
	obj->curanim=AnimBase[mCATERPIL]+mCATERPIL_IDLE;
	obj->objtype=oSPECIAL;


//CATERPILLAR SMOKE 
	obj=(Omonster *)(&loadobj[aCATSMOKE]);
	obj->ProcPtr=&cSMOKEPART;
	obj->xspeed=65536;
	obj->yspeed=-32768;
	obj->yacc=-256;//-1024;
	//obj->energy=0;
	obj->nohit=1; //no collision checking
	obj->loadanim=mCATERPIL;
	obj->curanim=AnimBase[mCATERPIL]+mCATERPIL_SMOKE;
	obj->objtype=oSPECIAL;//PARTICLE;



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

//CARROTUS POLE
	obj=(Omonster *)(&loadobj[aCARROTUSPOLE]);
	obj->ProcPtr=&rottree;
//	obj->nohit=1;
	obj->loadanim=mCARROTPOLE;
	obj->objtype=oSPECIAL+oTRIGGERTNT+oALWAYSACTIVE;
	obj->curanim=AnimBase[mCARROTPOLE]+mCARROTPOLE_POLE;

//JUNGLE POLE
	obj=(Omonster *)(&loadobj[aJUNGLEPOLE]);
	obj->ProcPtr=&rottree;
//	obj->nohit=1;
	obj->loadanim=mJUNGLEPOLE;
	obj->objtype=oSPECIAL+oTRIGGERTNT+oALWAYSACTIVE;
	obj->curanim=AnimBase[mJUNGLEPOLE]+mJUNGLEPOLE_POLE;


//AMBIENTSOUND
	obj=(Omonster *)(&loadobj[aAMBIENTSOUND]);
	obj->ProcPtr=&cAMBIENT;
	obj->objtype=oSPECIAL;

//UTERUSEL
	obj=(Omonster *)(&loadobj[aUTERUSEL]);
	obj->ProcPtr=&cUTERUSEL;
	obj->energy=3;
	obj->loadanim=mUTERUS;
	obj->objtype=oSPECIAL+oTRIGGERTNT+oENEMY;
	obj->curanim=AnimBase[mUTERUS]+mUTERUS_PART;
	obj->killanim=AnimBase[mAMMO]+mAMMO_BOOM6;

//UTERUS
	obj=(Omonster *)(&loadobj[aUTERUS]);
	obj->ProcPtr=&cUTERUS;
	obj->nohit=3;	//dont hit
	obj->energy=100;
	obj->loadanim=mUTERUS;
	obj->objtype=oSPECIAL+oTRIGGERTNT+oENEMY;
	obj->curanim=AnimBase[mUTERUS]+mUTERUS_FLY;
	obj->points=3000;


	SetupEnemies2();

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


void ReInitObjects(int nump)
{
	int num,x,y;
	Word32 *event;

/*	this flag lets objects know that the sDEACTIVATE state means they should restore
	because the whole level is reinitialized (not a sDEACTIVATE because of out-of-screen
	clipping
*/
	ReInitializingObjects=1;


	for (num=1;num<NumObjects;num++)
	if (gameobj[num].ProcPtr)	//can be deleted now!
	{
		gameobj[num].state=sDEACTIVATE;
		gameobj[num].ProcPtr(num);	//jump to code of object
		MyDeleteObject(num);//gameobj[num].code=NULL;
	}

	for (y=0; y<(long)Layer_Height[SPRITELAYER]; y++)
	{
		event=EventMap[y];
		for (x=0; x<(long)Layer_Width[SPRITELAYER]; x++)
		{
			*event=(*event)&(0x0ffffffff^0x800);
			event++; //incs with 4!
		}
	}

	ReInitializingObjects=0;

	FastMemSet(TriggerSceneryActive,0,sizeof(TriggerSceneryActive));
} //ReInitObjects()


static void UnPackTexture(int base,int sub)
{
	int num,t,frame;
	Word8 *oldframe;
#if _DEBUG
	int memoryreq=0;
#endif
	int calc;


	if (AnimBase[base]) {	//loaded!
		num=AnimBase[base]+sub;
		for (t=0;t<anims[num].numframes;t++) {
			void *DestPtr;
			frame=anims[num].framelist[t];

			calc=128*(1+frames[frame].dy);

#if _DEBUG
			memoryreq+=calc;
#endif
			//ugly:
			oldframe=frames[frame].data; //this block disappears! UGLY!
				
			DestPtr=AllocAPointer(calc);
			frames[frame].data = (Word8*)DestPtr;
			AddDeadPointer(DestPtr);
			FastMemSet(DestPtr,0,calc); //to be sure

			sUnRLE128Sprite(oldframe,frames[frame].data);	
		}
#if _DEBUG
		DebugXMessage("mem req for this rot sprite: %u\n",memoryreq);
#endif
	}
	
} //UnPackTexture()



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
