#define DEBUG testmode
// Built in a $DEBUG$ release

#ifdef DEBUG
// DEBUG is defined!
int x = 5;
#endif

#ifndef DEBUG
// DEBUG is not defined!
int x = 2;
#endif

#defmacro gaming amount_of_gaming epics
// this is the gaming macro
int xy = %amount_of_gaming%; // this has %epics% epics!!!!!!
int yp = xy*3;
#enddef

#macro gaming 4 6
#macro gaming "testing_string" yah 

jjConsole("$DEBUG$");

#undef DEBUG

#ifdef DEBUG
// we '#undef'ed DEBUG, and so this shouldnt appear!
#endif

