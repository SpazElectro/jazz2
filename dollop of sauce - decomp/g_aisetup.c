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
