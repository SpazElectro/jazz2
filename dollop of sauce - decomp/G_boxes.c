/*
	G_BOXES.C

	routines for collision with solid boxes
*/
#include "G_Boxes.h"
#include "Random.h"
#include "E_Loaders.h"
#include "G_PlayerAnim.h"
#include "G_Collision.h"
#include "G_Ai.h"
#include "G_Bullet.h"
#include "G_Objects.h"
#include "Globals.h"

#define PUSHSPEED (3*8192)
#define BOXFUDGEX (6*8192)
