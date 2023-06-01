// player
enum Player_Character {
    mJAZZ = 0,
    mSPAZ = 1,
    mLORI = 2,
    mBIRD = 3,
    mFROG = 4
}

enum Player_Downattack {
    DOWNATTACKLEN = 0,
    DOWNATTACKWAIT = 1
}

struct Player_MOVE {
    int Hdir;
}

struct Player_Display {
    int State;
    int Var1;
    int Counter;
    int Xpos;
    int Ypos;
    char* Text;
}

struct Player {
    int xpos;
    int ypos;
    int platform;
    int platformtype;
    int rolling;
    int lastjump;
    int yacc;
    int stop;
    int bemoved;
    int xspeed;
    int yspeed;
    int spring;
    int pushobject;
    int ShiftPositionX;
    int movexspeed;
    int moveyspeed;
    int idletime;
    Player_Downattack downattack;
    int rundash;
    int specialjump;
    int lastspring;
    Player_Character character;
    bool localplayer;
    bool Active;
    int[] oldgem;
    int[] oldammo;
    int[] oldgunpower;
    int[] gem;
    int lastgemcounter;
    Player_Display[] Display; // seems important; has a lot of stuff that look like either
                            // a player class, or an object classs
    int viewstartx;
    int viewstarty;
    int warparea;
    int warprandomizer;
    int fly;
    int lapcounter;
    int lastlapfinish;
    bool dive;
    int curframe;
    int direction;
    bool swim;
    int firedirection;
    int[] gunpower;
    int shieldtype;
    int firetype;
    int fireangle;
    int[] ammo;
    int rush;
    int flicker;
    int bird;
    int invincibility;
    int hit;
    int shield;
    int energy;
    int flag;
    int lives;
    int morph;
    int LastReceivedGameCounter;
    int ErrorStep;
    bool godown;
    int xacc;
    Player_MOVE MOVE;
    bool goleft;
    bool goright;
    bool goup;
    int vpole;
    int hpole;
    bool run;
    int rundash;
    int helicopter;
    bool hang;
    int slope;
    bool gofardown;
    bool ledgewiggle;
    int QuakeX;
    int lastevent;
    int frogmorph;
    int lastsuckpos;
    int sucked;
    int fixstartx;
    int fixscrollx;
    int lastpolepos;
    int polespeed;
    int AmbientBack;
    int vine;
    int stoned;
    int stonedLen;
    // int netcollectgem;
    int food;
    int var1;
    int berserk;
    int autofire;
    int fastfeet;
    int time;
    int freeze;
};
// player
