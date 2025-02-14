#include "../../experiments/spazlint2/include/as.predefined"

#define DEBUG testmode

#ifdef DEBUG
// DEBUG is defined!
int x = 5;
#endif

#ifndef DEBUG
// DEBUG is not defined!
int xv = 2;
#endif

#defmacro gaming amount_of_gaming epics
// this is the gaming macro
int xy = $amount_of_gaming$; // this has $epics$ epics!!!!!!
int yp = xy*3;
#enddef

#macro gaming 4 6
#macro gaming "testing_string" yah 

void main() {
    jjConsole("$DEBUG$");
}

#undef DEBUG

#ifdef DEBUG
// we '#undef'ed DEBUG, and so this shouldnt appear!
#endif


#ifdef DEBUG
// again, undef

#macro gaming 3 2

#else
// yay this should appear because this is an else statement
#macro gaming 6 9

#endif

