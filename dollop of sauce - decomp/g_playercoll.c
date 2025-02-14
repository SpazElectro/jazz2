/*
	G_PLAYERCOLL.C


	player collision
*/
#include "G_PlayerColl.h"
#include "M_SaveLoad.h"
#include "E_Sound.h"
#include "Random.h"
#include "E_Strings.h"
#include "G_Error.h"
#include "G_Event.h"
#include "E_Loaders.h"
#include "Globals.h"
#include "N_Main.h"
#include "G_Hit.h"
#include "G_PlayerAnim.h"
#include "G_Collision.h"
#include "G_Objects.h"
#include "G_Bosses.h"
#include "G_Ai.h"
#include "E_Particles.h"
#include "G_Stat.h"
#include "G_Bullet.h"
#include "G_AiTypes.h"
#include "G_Find.h"
#include "G_Init.h"

int buttstompsounds[4]=
{	sCOMMON_SPLAT1,sCOMMON_SPLAT2,sCOMMON_SPLAT3,sCOMMON_SPLAT4};

/*
	player2playercol	


  the client does this for his own players only

  when you hit another player
    client-> sent stomp command

    server -> sent hitplayer
*/
