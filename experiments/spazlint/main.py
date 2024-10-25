import json, os
from typing import Tuple
import errorchecker, hierachy2

globalProperties: dict = json.load(open(os.path.dirname(__file__) + "/global.json"))
classProperties: dict = json.load(open(os.path.dirname(__file__) + "/classes.json"))
_eventsList = globalProperties["eventsList"]
eventsList = []

for x in _eventsList:
    eventsList.append(x["name"])

del _eventsList

# merge globalProperties into one
globalPropertiesTemp = {}
for i in globalProperties:
    globalPropertiesTemp[i[:-4]] = globalProperties[i]

classPropertiesTemp = {}
for i in classProperties:
    classPropertiesTemp[i[:-4]] = classProperties[i]

globalProperties = globalPropertiesTemp
classProperties = classPropertiesTemp

# TODO: make this portable so I don't have to copy paste from scrape.js and into here
ENUM_ARRAY = {
    "CREATOR::Type": ["OBJECT", "LEVEL", "PLAYER"],
    "PARTICLE::Type": ["INACTIVE", "FIRE", "FLOWER", "ICETRAIL", "LEAF", "PIXEL", "RAIN", "SMOKE", "SNOW", "SPARK", "STAR", "STRING", "TILE"],
    "STRING::Size": ["SMALL", "MEDIUM", "LARGE"],
    "STRING::SIZE": ["SMALL", "MEDIUM", "LARGE"], # the plus docs use `Size` and `SIZE`, I'm triggered.
    "SPRITE::Mode": ["ALPHAMAP", "BLEND_NORMAL", "BLEND_DISSOLVE", "BLEND_DARKEN", "BLEND_MULTIPLY", "BLEND_BURN", "BLEND_LIGHTEN", "BLEND_SCREEN", "BLEND_DODGE", "BLEND_OVERLAY", "BLEND_SOFTLIGHT", "BLEND_HARDLIGHT", "BLEND_DIFFERENCE", "BLEND_EXCLUSION", "BLEND_COLOR", "BLEND_HUE", "BLEND_SATURATION", "BLEND_LUMINANCE", "BRIGHTNESS", "CHROMAKEY", "FROZEN", "GEM", "INVISIBLE", "MAPPING", "MENUPLAYER", "NEONGLOW", "NORMAL", "PALSHIFT", "PLAYER", "RESIZED", "SHADOW", "SINGLECOLOR", "SINGLEHUE", "TINTED", "TRANSLUCENT", "TRANSLUCENTCOLOR", "TRANSLUCENTMAPPING", "TRANSLUCENTPALSHIFT", "TRANSLUCENTPLAYER", "TRANSLUCENTSINGLEHUE", "TRANSLUCENTTILE"],
    "TILE::Quadrant": ["TOPLEFT", "TOPRIGHT", "BOTTOMLEFT", "BOTTOMRIGHT", "ALLQUADRANTS"],
    "SOUND::Sample": ["AMMO_BLUB1", "AMMO_BLUB2", "AMMO_BMP1", "AMMO_BMP2", "AMMO_BMP3", "AMMO_BMP4", "AMMO_BMP5", "AMMO_BMP6", "AMMO_BOEM1", "AMMO_BUL1", "AMMO_BULFL1", "AMMO_BULFL2", "AMMO_BULFL3", "AMMO_FIREGUN1A", "AMMO_FIREGUN2A", "AMMO_FUMP", "AMMO_GUN1", "AMMO_GUN2", "AMMO_GUN3PLOP", "AMMO_GUNFLP", "AMMO_GUNFLP1", "AMMO_GUNFLP2", "AMMO_GUNFLP3", "AMMO_GUNFLP4", "AMMO_GUNFLPL", "AMMO_GUNJAZZ", "AMMO_GUNVELOCITY", "AMMO_ICEGUN", "AMMO_ICEGUN2", "AMMO_ICEGUNPU", "AMMO_ICEPU1", "AMMO_ICEPU2", "AMMO_ICEPU3", "AMMO_ICEPU4", "AMMO_LASER", "AMMO_LASER2", "AMMO_LASER3", "AMMO_LAZRAYS", "AMMO_MISSILE", "AMMO_SPZBL1", "AMMO_SPZBL2", "AMMO_SPZBL3", "BAT_BATFLY1", "BILSBOSS_BILLAPPEAR", "BILSBOSS_FINGERSNAP", "BILSBOSS_FIRE", "BILSBOSS_FIRESTART", "BILSBOSS_SCARY3", "BILSBOSS_THUNDER", "BILSBOSS_ZIP", "BONUS_BONUS1", "BONUS_BONUSBLUB", "BUBBA_BUBBABOUNCE1", "BUBBA_BUBBABOUNCE2", "BUBBA_BUBBAEXPLO", "BUBBA_FROG2", "BUBBA_FROG3", "BUBBA_FROG4", "BUBBA_FROG5", "BUBBA_SNEEZE2", "BUBBA_TORNADOATTACK2", "BUMBEE_BEELOOP", "CATERPIL_RIDOE", "COMMON_AIRBOARD", "COMMON_AIRBTURN", "COMMON_AIRBTURN2", "COMMON_BASE1", "COMMON_BELL_FIRE", "COMMON_BELL_FIRE2", "COMMON_BENZIN1", "COMMON_BIRDFLY", "COMMON_BIRDFLY2", "COMMON_BLOKPLOP", "COMMON_BLUB1", "COMMON_BUBBLGN1", "COMMON_BURN", "COMMON_BURNIN", "COMMON_CANSPS", "COMMON_CLOCK", "COMMON_COIN", "COMMON_COLLAPS", "COMMON_CUP", "COMMON_DAMPED1", "COMMON_DOWN", "COMMON_DOWNFL2", "COMMON_DRINKSPAZZ1", "COMMON_DRINKSPAZZ2", "COMMON_DRINKSPAZZ3", "COMMON_DRINKSPAZZ4", "COMMON_EAT1", "COMMON_EAT2", "COMMON_EAT3", "COMMON_EAT4", "COMMON_ELECTRIC1", "COMMON_ELECTRIC2", "COMMON_ELECTRICHIT", "COMMON_EXPL_TNT", "COMMON_EXPSM1", "COMMON_FLAMER", "COMMON_FLAP", "COMMON_FOEW1", "COMMON_FOEW2", "COMMON_FOEW3", "COMMON_FOEW4", "COMMON_FOEW5", "COMMON_GEMSMSH1", "COMMON_GLASS2", "COMMON_GUNSM1", "COMMON_HARP1", "COMMON_HEAD", "COMMON_HELI1", "COMMON_HIBELL", "COMMON_HOLYFLUT", "COMMON_HORN1", "COMMON_ICECRUSH", "COMMON_IMPACT1", "COMMON_IMPACT2", "COMMON_IMPACT3", "COMMON_IMPACT4", "COMMON_IMPACT5", "COMMON_IMPACT6", "COMMON_IMPACT7", "COMMON_IMPACT8", "COMMON_IMPACT9", "COMMON_ITEMTRE", "COMMON_JUMP", "COMMON_JUMP2", "COMMON_LAND", "COMMON_LAND1", "COMMON_LAND2", "COMMON_LANDCAN1", "COMMON_LANDCAN2", "COMMON_LANDPOP", "COMMON_LOADJAZZ", "COMMON_LOADSPAZ", "COMMON_METALHIT", "COMMON_MONITOR", "COMMON_NOCOIN", "COMMON_PICKUP1", "COMMON_PICKUPW1", "COMMON_PISTOL1", "COMMON_PLOOP1", "COMMON_PLOP1", "COMMON_PLOP2", "COMMON_PLOP3", "COMMON_PLOP4", "COMMON_PLOPKORK", "COMMON_PREEXPL1", "COMMON_PREHELI", "COMMON_REVUP", "COMMON_RINGGUN", "COMMON_RINGGUN2", "COMMON_SHIELD1", "COMMON_SHIELD4", "COMMON_SHIELD_ELEC", "COMMON_SHLDOF3", "COMMON_SLIP", "COMMON_SMASH", "COMMON_SPLAT1", "COMMON_SPLAT2", "COMMON_SPLAT3", "COMMON_SPLAT4", "COMMON_SPLUT", "COMMON_SPRING1", "COMMON_STEAM", "COMMON_STEP", "COMMON_STRETCH", "COMMON_SWISH1", "COMMON_SWISH2", "COMMON_SWISH3", "COMMON_SWISH4", "COMMON_SWISH5", "COMMON_SWISH6", "COMMON_SWISH7", "COMMON_SWISH8", "COMMON_TELPORT1", "COMMON_TELPORT2", "COMMON_UP", "COMMON_WATER", "COMMON_WOOD1", "DEMON_RUN", "DEVILDEVAN_DRAGONFIRE", "DEVILDEVAN_FLAP", "DEVILDEVAN_FROG4", "DEVILDEVAN_JUMPUP", "DEVILDEVAN_LAUGH", "DEVILDEVAN_PHASER2", "DEVILDEVAN_STRECh3", "DEVILDEVAN_STRECHTAIL", "DEVILDEVAN_STRETCH1", "DEVILDEVAN_STRETCH3", "DEVILDEVAN_VANISH1", "DEVILDEVAN_WHISTLEDESCENDING2", "DEVILDEVAN_WINGSOUT", "DOG_AGRESSIV", "DOG_SNIF1", "DOG_WAF1", "DOG_WAF2", "DOG_WAF3", "DRAGFLY_BEELOOP", "ENDING_OHTHANK", "ENDTUNEJAZZ_TUNE", "ENDTUNELORI_CAKE", "ENDTUNESPAZ_TUNE", "EPICLOGO_EPIC1", "EPICLOGO_EPIC2", "EVA_KISS1", "EVA_KISS2", "EVA_KISS3", "EVA_KISS4", "FAN_FAN", "FATCHK_HIT1", "FATCHK_HIT2", "FATCHK_HIT3", "FENCER_FENCE1", "FROG_FROG", "FROG_FROG1", "FROG_FROG2", "FROG_FROG3", "FROG_FROG4", "FROG_FROG5", "FROG_JAZZ2FROG", "FROG_TONG", "GLOVE_HIT", "HATTER_CUP", "HATTER_HAT", "HATTER_PTOEI", "HATTER_SPLIN", "HATTER_SPLOUT", "INTRO_BLOW", "INTRO_BOEM1", "INTRO_BOEM2", "INTRO_BRAKE", "INTRO_END", "INTRO_GRAB", "INTRO_GREN1", "INTRO_GREN2", "INTRO_GREN3", "INTRO_GUNM0", "INTRO_GUNM1", "INTRO_GUNM2", "INTRO_HELI", "INTRO_HITSPAZ", "INTRO_HITTURT", "INTRO_IFEEL", "INTRO_INHALE", "INTRO_INSECT", "INTRO_KATROL", "INTRO_LAND", "INTRO_MONSTER", "INTRO_MONSTER2", "INTRO_ROCK", "INTRO_ROPE1", "INTRO_ROPE2", "INTRO_RUN", "INTRO_SHOT1", "INTRO_SHOTGRN", "INTRO_SKI", "INTRO_STRING", "INTRO_SWISH1", "INTRO_SWISH2", "INTRO_SWISH3", "INTRO_SWISH4", "INTRO_UHTURT", "INTRO_UP1", "INTRO_UP2", "INTRO_WIND_01", "JAZZSOUNDS_BALANCE", "JAZZSOUNDS_HEY1", "JAZZSOUNDS_HEY2", "JAZZSOUNDS_HEY3", "JAZZSOUNDS_HEY4", "JAZZSOUNDS_IDLE", "JAZZSOUNDS_JAZZV1", "JAZZSOUNDS_JAZZV2", "JAZZSOUNDS_JAZZV3", "JAZZSOUNDS_JAZZV4", "JAZZSOUNDS_JUMMY", "JAZZSOUNDS_PFOE", "LABRAT_BITE", "LABRAT_EYE2", "LABRAT_EYE3", "LABRAT_MOUSE1", "LABRAT_MOUSE2", "LABRAT_MOUSE3", "LIZARD_LIZ1", "LIZARD_LIZ2", "LIZARD_LIZ4", "LIZARD_LIZ6", "LORISOUNDS_DIE1", "LORISOUNDS_HURT0", "LORISOUNDS_HURT1", "LORISOUNDS_HURT2", "LORISOUNDS_HURT3", "LORISOUNDS_HURT4", "LORISOUNDS_HURT5", "LORISOUNDS_HURT6", "LORISOUNDS_HURT7", "LORISOUNDS_LORI1", "LORISOUNDS_LORI2", "LORISOUNDS_LORIBOOM", "LORISOUNDS_LORIFALL", "LORISOUNDS_LORIJUMP", "LORISOUNDS_LORIJUMP2", "LORISOUNDS_LORIJUMP3", "LORISOUNDS_LORIJUMP4", "LORISOUNDS_TOUCH", "LORISOUNDS_WEHOO", "MENUSOUNDS_SELECT0", "MENUSOUNDS_SELECT1", "MENUSOUNDS_SELECT2", "MENUSOUNDS_SELECT3", "MENUSOUNDS_SELECT4", "MENUSOUNDS_SELECT5", "MENUSOUNDS_SELECT6", "MENUSOUNDS_TYPE", "MENUSOUNDS_TYPEENTER", "MONKEY_SPLUT", "MONKEY_THROW", "MOTH_FLAPMOTH", "ORANGE_BOEML", "ORANGE_BOEMR", "ORANGE_BUBBELSL", "ORANGE_BUBBELSR", "ORANGE_GLAS1L", "ORANGE_GLAS1R", "ORANGE_GLAS2L", "ORANGE_GLAS2R", "ORANGE_MERGE", "ORANGE_SWEEP0L", "ORANGE_SWEEP0R", "ORANGE_SWEEP1L", "ORANGE_SWEEP1R", "ORANGE_SWEEP2L", "ORANGE_SWEEP2R", "P2_CRUNCH", "P2_FART", "P2_FOEW1", "P2_FOEW4", "P2_FOEW5", "P2_FROG1", "P2_FROG2", "P2_FROG3", "P2_FROG4", "P2_FROG5", "P2_KISS4", "P2_OPEN", "P2_PINCH1", "P2_PINCH2", "P2_PLOPSEQ1", "P2_PLOPSEQ2", "P2_PLOPSEQ3", "P2_PLOPSEQ4", "P2_POEP", "P2_PTOEI", "P2_SPLOUT", "P2_SPLUT", "P2_THROW", "P2_TONG", "PICKUPS_BOING_CHECK", "PICKUPS_HELI2", "PICKUPS_STRETCH1A", "PINBALL_BELL", "PINBALL_FLIP1", "PINBALL_FLIP2", "PINBALL_FLIP3", "PINBALL_FLIP4", "QUEEN_LADYUP", "QUEEN_SCREAM", "RAPIER_GOSTDIE", "RAPIER_GOSTLOOP", "RAPIER_GOSTOOOH", "RAPIER_GOSTRIP", "RAPIER_HITCHAR", "ROBOT_BIG1", "ROBOT_BIG2", "ROBOT_CAN1", "ROBOT_CAN2", "ROBOT_HYDRO", "ROBOT_HYDRO2", "ROBOT_HYDROFIL", "ROBOT_HYDROPUF", "ROBOT_IDLE1", "ROBOT_IDLE2", "ROBOT_JMPCAN1", "ROBOT_JMPCAN10", "ROBOT_JMPCAN2", "ROBOT_JMPCAN3", "ROBOT_JMPCAN4", "ROBOT_JMPCAN5", "ROBOT_JMPCAN6", "ROBOT_JMPCAN7", "ROBOT_JMPCAN8", "ROBOT_JMPCAN9", "ROBOT_METAL1", "ROBOT_METAL2", "ROBOT_METAL3", "ROBOT_METAL4", "ROBOT_METAL5", "ROBOT_OPEN", "ROBOT_OUT", "ROBOT_POEP", "ROBOT_POLE", "ROBOT_SHOOT", "ROBOT_STEP1", "ROBOT_STEP2", "ROBOT_STEP3", "ROCK_ROCK1", "RUSH_RUSH", "SCIENCE_PLOPKAOS", "SKELETON_BONE1", "SKELETON_BONE2", "SKELETON_BONE3", "SKELETON_BONE5", "SKELETON_BONE6", "SKELETON_BONE7", "SMALTREE_FALL", "SMALTREE_GROUND", "SMALTREE_HEAD", "SONCSHIP_METAL1", "SONCSHIP_MISSILE2", "SONCSHIP_SCRAPE", "SONCSHIP_SHIPLOOP", "SONCSHIP_TARGETLOCK", "SPAZSOUNDS_AUTSCH1", "SPAZSOUNDS_AUTSCH2", "SPAZSOUNDS_BIRDSIT", "SPAZSOUNDS_BURP", "SPAZSOUNDS_CHIRP", "SPAZSOUNDS_EATBIRD", "SPAZSOUNDS_HAHAHA", "SPAZSOUNDS_HAHAHA2", "SPAZSOUNDS_HAPPY", "SPAZSOUNDS_HIHI", "SPAZSOUNDS_HOHOHO1", "SPAZSOUNDS_HOOO", "SPAZSOUNDS_KARATE7", "SPAZSOUNDS_KARATE8", "SPAZSOUNDS_OHOH", "SPAZSOUNDS_OOOH", "SPAZSOUNDS_WOOHOO", "SPAZSOUNDS_YAHOO", "SPAZSOUNDS_YAHOO2", "SPRING_BOING_DOWN", "SPRING_SPRING1", "STEAM_STEAM", "STONED_STONED", "SUCKER_FART", "SUCKER_PINCH1", "SUCKER_PINCH2", "SUCKER_PINCH3", "SUCKER_PLOPSEQ1", "SUCKER_PLOPSEQ2", "SUCKER_PLOPSEQ3", "SUCKER_PLOPSEQ4", "SUCKER_UP", "TUFBOSS_CATCH", "TUFBOSS_RELEASE", "TUFBOSS_SWING", "TURTLE_BITE3", "TURTLE_HIDE", "TURTLE_HITSHELL", "TURTLE_IDLE1", "TURTLE_IDLE2", "TURTLE_NECK", "TURTLE_SPK1TURT", "TURTLE_SPK2TURT", "TURTLE_SPK3TURT", "TURTLE_SPK4TURT", "TURTLE_TURN", "UTERUS_CRABCLOSE", "UTERUS_CRABOPEN2", "UTERUS_SCISSORS1", "UTERUS_SCISSORS2", "UTERUS_SCISSORS3", "UTERUS_SCISSORS4", "UTERUS_SCISSORS5", "UTERUS_SCISSORS6", "UTERUS_SCISSORS7", "UTERUS_SCISSORS8", "UTERUS_SCREAM1", "UTERUS_STEP1", "UTERUS_STEP2", "WIND_WIND2A", "WITCH_LAUGH", "WITCH_MAGIC", "XBILSY_BILLAPPEAR", "XBILSY_FINGERSNAP", "XBILSY_FIRE", "XBILSY_FIRESTART", "XBILSY_SCARY3", "XBILSY_THUNDER", "XBILSY_ZIP", "XLIZARD_LIZ1", "XLIZARD_LIZ2", "XLIZARD_LIZ4", "XLIZARD_LIZ6", "XTURTLE_BITE3", "XTURTLE_HIDE", "XTURTLE_HITSHELL", "XTURTLE_IDLE1", "XTURTLE_IDLE2", "XTURTLE_NECK", "XTURTLE_SPK1TURT", "XTURTLE_SPK2TURT", "XTURTLE_SPK3TURT", "XTURTLE_SPK4TURT", "XTURTLE_TURN", "ZDOG_AGRESSIV", "ZDOG_SNIF1", "ZDOG_WAF1", "ZDOG_WAF2", "ZDOG_WAF3"],
    "BEHAVIOR::Behavior": ["BULLET", "SEEKERBULLET", "RFBULLET", "TOASTERBULLET", "PEPPERBULLET", "ELECTROBULLET", "BUMP", "PADDLE", "BIGOBJECT", "WALKINGENEMY", "DESTRUCTSCENERY", "ROCKETTURTLE", "ROCKETTURTLEPLUS", "BOLLYTOP", "BOLLYBOTTOM", "PLATFORM", "SPRING", "AMMO15", "MONITOR", "CRATE", "PICKUP", "DIAMONDSAREFOREVER", "FLAG", "INACTIVE", "DEFAULT", "MONKEYBULLET", "BILSYBULLET", "BOLLYBULLET", "BOLLYSPIKEBALL", "WITCHBULLET", "TUFBOSSBULLET", "ROBOTSHARD", "BONE", "EXPLOSION2", "BURNING", "AIRBOARDFALL", "BIRDFEATHER", "UFO", "CORPSE"],
    "DIRECTION::Dir": ["RIGHT", "LEFT", "UP", "CURRENT"],
    "CHAR::Char": ["JAZZ", "SPAZ", "LORI", "BIRD", "FROG", "BIRD2"],
    "HANDLING::Player": ["PICKUP", "ENEMY", "SPECIAL", "PICKUP", "ENEMYBULLET", "PLAYERBULLET"],
    "GEM::Color": ["RED", "GREEN", "BLUE", "PURPLE"],
    "STRING::Mode": ["NORMAL", "DARK", "RIGHTALIGN", "BOUNCE", "SPIN", "PALSHIFT"],
    "CHAT::Type": ["NORMAL", "TEAMCHAT", "WHISPER", "ME"],
    "OBJECT::Object": ["BLASTERBULLET", "BOUNCERBULLET", "ICEBULLET", "SEEKERBULLET", "RFBULLET", "TOASTERBULLET", "FIREBALLBULLET", "ELECTROBULLET", "BLASTERBULLETPU", "BOUNCERBULLETPU", "ICEBULLETPU", "SEEKERBULLETPU", "RFBULLETPU", "TOASTERBULLETPU", "FIREBALLBULLETPU", "ELECTROBULLETPU", "FIRESHIELDBULLET", "WATERSHIELDBULLET", "BUBBLESHIELDBULLET", "LIGHTNINGSHIELDBULLET", "PLASMASHIELDBULLET", "BULLET", "SMOKERING", "SHARD", "EXPLOSION", "BOUNCEONCE", "FLICKERGEM", "LASER", "UTERUSSPIKEBALL", "BIRD", "BUBBLE", "ICEAMMO3", "BOUNCERAMMO3", "SEEKERAMMO3", "RFAMMO3", "TOASTERAMMO3", "TNTAMMO3", "GUN8AMMO3", "GUN9AMMO3", "TURTLESHELL", "SWINGINGVINE", "BOMB", "SILVERCOIN", "GOLDCOIN", "GUNCRATE", "CARROTCRATE", "ONEUPCRATE", "GEMBARREL", "CARROTBARREL", "ONEUPBARREL", "BOMBCRATE", "ICEAMMO15", "BOUNCERAMMO15", "SEEKERAMMO15", "RFAMMO15", "TOASTERAMMO15", "TNT", "AIRBOARDGENERATOR", "FROZENSPRING", "FASTFIRE", "SPRINGCRATE", "REDGEM", "GREENGEM", "BLUEGEM", "PURPLEGEM", "SUPERGEM", "BIRDCAGE", "GUNBARREL", "GEMCRATE", "MORPH", "CARROT", "FULLENERGY", "FIRESHIELD", "WATERSHIELD", "BUBBLESHIELD", "LIGHTNINGSHIELD", "PLASMASHIELD", "FASTFEET", "ONEUP", "EXTRALIFE", "EXTRALIVE", "EOLPOST", "SAVEPOST", "CHECKPOINT", "BONUSPOST", "REDSPRING", "GREENSPRING", "BLUESPRING", "INVINCIBILITY", "EXTRATIME", "FREEZER", "FREEZEENEMIES", "HORREDSPRING", "HORGREENSPRING", "HORBLUESPRING", "BIRDMORPH", "TRIGGERCRATE", "FLYCARROT", "RECTREDGEM", "RECTGREENGEM", "RECTBLUEGEM", "TUFTURT", "TUFBOSS", "LABRAT", "DRAGON", "LIZARD", "BEE", "BUMBEE", "RAPIER", "SPARK", "BAT", "SUCKER", "CATERPILLAR", "CHESHIRE1", "CHESHIRE2", "HATTER", "BILSY", "SKELETON", "DOGGYDOGG", "NORMTURTLE", "HELMUT", "DEMON", "DRAGONFLY", "MONKEY", "FATCHICK", "FENCER", "FISH", "MOTH", "STEAM", "ROTATINGROCK", "BLASTERPOWERUP", "BOUNCERPOWERUP", "ICEPOWERUP", "SEEKERPOWERUP", "RFPOWERUP", "TOASTERPOWERUP", "LEFTPADDLE", "RIGHTPADDLE", "FIVEHUNDREDBUMP", "CARROTBUMP", "APPLE", "BANANA", "CHERRY", "ORANGE", "PEAR", "PRETZEL", "STRAWBERRY", "STEADYLIGHT", "PULZELIGHT", "PULSELIGHT", "FLICKERLIGHT", "QUEEN", "FLOATSUCKER", "BRIDGE", "LEMON", "LIME", "THING", "WATERMELON", "PEACH", "GRAPES", "LETTUCE", "EGGPLANT", "CUCUMB", "CUCUMBER", "COKE", "SOFTDRINK", "PEPSI", "SODAPOP", "MILK", "PIE", "CAKE", "DONUT", "CUPCAKE", "CHIPS", "CANDY", "CHOCBAR", "CHOCOLATEBAR", "ICECREAM", "BURGER", "PIZZA", "FRIES", "CHICKENLEG", "SANDWICH", "TACO", "WEENIE", "HAM", "CHEESE", "FLOATLIZARD", "STANDMONKEY", "DESTRUCTSCENERY", "DESTRUCTSCENERYBOMB", "TNTDESTRUCTSCENERY", "COLLAPSESCENERY", "STOMPSCENERY", "GEMSTOMP", "RAVEN", "TUBETURTLE", "GEMRING", "SMALLTREE", "AMBIENTSOUND", "UTERUS", "CRAB", "WITCH", "ROCKETTURTLE", "BUBBA", "DEVILDEVAN", "DEVANROBOT", "ROBOT", "CARROTUSPOLE", "PSYCHPOLE", "DIAMONDUSPOLE", "FRUITPLATFORM", "BOLLPLATFORM", "GRASSPLATFORM", "PINKPLATFORM", "SONICPLATFORM", "SPIKEPLATFORM", "SPIKEBOLL", "GENERATOR", "EVA", "BUBBLER", "TNTPOWERUP", "GUN8POWERUP", "GUN9POWERUP", "SPIKEBOLL3D", "SPRINGCORD", "BEES", "COPTER", "LASERSHIELD", "STOPWATCH", "JUNGLEPOLE", "WARP", "BIGROCK", "BIGBOX", "TRIGGERSCENERY", "BOLLY", "BUTTERFLY", "BEEBOY", "SNOW", "TWEEDLEBOSS", "AIRBOARD", "CTFBASE", "XMASNORMTURTLE", "XMASLIZARD", "XMASFLOATLIZARD", "XMASBILSY", "CAT", "PACMANGHOST"],
    "AREA::Area": ["ONEWAY", "HURT", "VINE", "HOOK", "SLIDE", "HPOLE", "VPOLE", "FLYOFF", "RICOCHET", "BELTRIGHT", "BELTLEFT", "ACCBELTRIGHT", "ACCBELTLEFT", "STOPENEMY", "WINDLEFT", "WINDRIGHT", "EOL", "WARPEOL", "REVERTMORPH", "FLOATUP", "TRIGGERROCK", "DIMLIGHT", "SETLIGHT", "LIMITXSCROLL", "RESETLIGHT", "WARPSECRET", "ECHO", "ACTIVATEBOSS", "JAZZLEVELSTART", "JAZZSTART", "SPAZLEVELSTART", "SPAZSTART", "MPLEVELSTART", "MPSTART", "LORILEVELSTART", "LORISTART", "WARP", "WARPTARGET", "PATH", "AREAID", "NOFIREZONE", "TRIGGERZONE", "SUCKERTUBE", "TEXT", "WATERLEVEL", "MORPHFROG", "WATERBLOCK"],
    "ANIM::Set": ["AMMO", "BAT", "BEEBOY", "BEES", "BIGBOX", "BIGROCK", "BIGTREE", "BILSBOSS", "BIRD", "BIRD3D", "BOLLPLAT", "BONUS", "BOSS", "BRIDGE", "BUBBA", "BUMBEE", "BUTTERFLY", "CARROTPOLE", "CAT", "CAT2", "CATERPIL", "CHUCK", "COMMON", "CONTINUE", "DEMON", "DESTSCEN", "DEVAN", "DEVILDEVAN", "DIAMPOLE", "DOG", "DOOR", "DRAGFLY", "DRAGON", "EVA", "FACES", "FATCHK", "FENCER", "FISH", "FLAG", "FLARE", "FONT", "FROG", "FRUITPLAT", "GEMRING", "GLOVE", "GRASSPLAT", "HATTER", "HELMUT", "JAZZ", "JAZZ3D", "JUNGLEPOLE", "LABRAT", "LIZARD", "LORI", "LORI2", "MENU", "MENUFONT", "MONKEY", "MOTH", "PICKUPS", "PINBALL", "PINKPLAT", "PSYCHPOLE", "QUEEN", "RAPIER", "RAVEN", "ROBOT", "ROCK", "ROCKTURT", "SKELETON", "SMALTREE", "SNOW", "SONCSHIP", "SONICPLAT", "SPARK", "SPAZ", "SPAZ2", "SPAZ3D", "SPIKEBOLL", "SPIKEBOLL3D", "SPIKEPLAT", "SPRING", "STEAM", "SUCKER", "TUBETURT", "TUFBOSS", "TUFTUR", "TURTLE", "TWEEDLE", "UTERUS", "VINE", "WARP10", "WARP100", "WARP20", "WARP50", "WITCH", "XBILSY", "XLIZARD", "XTURTLE", "ZDOG", "ZSPARK"],
    "COLOR::Component": ["RED", "GREEN", "BLUE"],
    "TEXTURE::Texture": ["BLADE", "CORRUPTEDSANCTUARY", "DESOLATION", "DIAMONDUSBETA", "ICTUBELECTRIC", "MEDIVO", "MEZ02", "MUCKAMOKNIGHT", "NORMAL", "PSYCH", "RANEFORUSV", "WINDSTORMFORTRESS", "WISETYNESS", "WTF", "XARGON"],
    "TEXTURE::Style": ["CYLINDER", "MENU", "REFLECTION", "TILEMENU", "TUNNEL", "WAVE", "WARPHORIZON"],
    "SURFACE::Surface": ["UNTEXTURED", "LEGACY", "FULLSREEN", "INNERWINDOW", "INNERLAYER"],
    "LAYERSPEEDMODEL::LayerSpeedModel": ["NORMAL", "LAYER8", "BOTHSPEEDS", "FROMSTART", "FITLEVEL", "SPEEDMULTIPLIERS"],
    "LIGHT::Enforce": ["OPTIONAL", "BASIC", "COMPLETE"],
    "SNOWING::Type": ["SNOW", "FLOWER", "RAIN", "LEAF"],
    "WATERINTERACTION::WaterInteraction": ["SWIM", "LOWGRAVITY", "POSITIONBASED"],
    "WATERLIGHT::wl": ["NONE", "GLOBAL", "LAGUNICUS"],
    "LIGHT::Type": ["NONE", "NORMAL", "POINT", "POINT2", "FLICKER", "BRIGHT", "LASERBEAM", "LASER", "RING", "RING2", "PLAYER"],
    "HANDLING::Bullet": ["HURTBYBULLET", "IGNOREBULLET", "DESTROYBULLET", "DETECTBULLET"],
    "STATE::State": ["START", "SLEEP", "WAKE", "KILL", "DEACTIVATE", "WALK", "JUMP", "FIRE", "FLY", "BOUNCE", "EXPLODE", "ROCKETFLY", "STILL", "FLOAT", "HIT", "SPRING", "ACTION", "DONE", "PUSH", "FALL", "FLOATFALL", "CIRCLE", "ATTACK", "FREEZE", "FADEIN", "FADEOUT", "HIDE", "TURN", "IDLE", "EXTRA", "STOP", "WAIT", "LAND", "DELAYEDSTART", "ROTATE", "DUCK"],
    "SPREAD::Spread": ["NORMAL", "ICEPU", "ICE", "RFNORMAL", "RFPU", "RF", "TOASTER", "PEPPERSPRAY", "GUN8"],
    "WEAPON::Style": ["NORMAL", "MISSILE", "POPCORN", "CAPPED"],
    "STRING::Alignment": ["DEFAULT", "LEFT", "CENTER", "RIGHT"],
    "STRING::SignTreatment": ["HIDESIGN", "DISPLAYSIGN", "SPECIALSIGN"],
    "TEAM::COLOR": ["NEUTRAL", "BLUE", "RED", "GREEN", "YELLOW"],
    "AIR::Jump": ["NONE", "HELICOPTER", "DOUBLEJUMP"],
    "GROUND::Jump": ["JAZZ", "SPAZ", "LORI", "CROUCH", "JUMP"]
}

# the most important thing is *good* syntax, what I mean is instead of going like:
# ```
# jjConsole(
#   "hello world"
# );
# ```
# you just do `jjConsole("hello world");`

class JJ2PlusLinter:
    def __init__(self, file):
        self.file = file
        os.makedirs(os.path.dirname(file), exist_ok=True)
        if os.path.exists(file):
            self.code = open(file).read()
        else:
            print("File was not found, created parent directory!")
            self.code = ""
        # self.enabled_errors = ["semicolons"]
        self.enabled_errors = []

        if len(self.code.splitlines()) > 0:
            disabled_errors = self.code.splitlines()[0]
            if disabled_errors.startswith("// @ignore-"):
                err = disabled_errors.split("// @ignore-")[1]

                if err in self.enabled_errors:
                    self.enabled_errors.remove(err)

    def lint(self, advanced=False):
        return []

        linting_errors = []

        # obviously this isnt gonna work that great but hey it gets the job done I guess
        # Rule: Check for missing semicolons at the end of lines
        lines = self.code.split("\n")

        if "semicolons" in self.enabled_errors:
            for line_index, line in enumerate(lines):
                line = line.strip()

                if line and not line.endswith(";"):
                    if line_index != 0 and lines[line_index - 1].strip() == "// @ignore-semicolons":
                        continue
                    if line == "" or line.endswith("{") or line.endswith("}") or \
                        line.startswith("#") or line.startswith("//") or \
                        line.startswith("if") or "//" in line or \
                        line.endswith(",") or line.endswith("(") or \
                        line.startswith("else") or line.endswith("else") or \
                        line.startswith("for") or (line.startswith("case") and line.endswith(":")) or \
                        line.startswith("default:") or line.endswith("&&"):
                        continue
                    if lines[line_index + 1].strip() == "{" or lines[line_index + 1].strip() == ");" or lines[line_index + 1].strip() == "};":
                        continue
                    if line_index != 0 and lines[line_index - 1].strip().startswith("for"):
                        continue

                    linting_errors.append({
                        "line": line_index,
                        "char": 0,
                        "text": f"Missing semicolon.",
                        "type": "ERR"
                    })
        
        if advanced:
            for e in errorchecker.getErrors(self.file):
                linting_errors.append({
                    "line": int(e.split("(")[1].split(", ")[0].split(") : ")[0]) - 1,
                    "char": int(e.split("(")[1].split(", ")[1].split(") : ")[0]) - 1,
                    "text": e.split("(")[1].split(") : ")[1].split(" : ")[1],
                    "type": e.split("(")[1].split(") : ")[1].split(" : ")[0]
                })
        
        return linting_errors

    def autocomplete(self, line_number, char):
        # print(f"JJ2PlusLinter.autocomplete(line_number={line_number}, char={char})")
        if isinstance(line_number, str):
            line_number = int(line_number)
        if isinstance(char, str):
            char = int(char)
        if self.code == "":
            print("Empty code, file writing has (possibly) not finished!")
            return []
        split_lines = self.code.splitlines()
        if line_number <= 0 or line_number > len(split_lines):
            print(f"Invalid line number (specified: {line_number} when only has: {len(split_lines)})")
            return []
        line = split_lines[line_number - 1].strip().lower()
        suggestions = []

        fnc = hierachy2.find_function(split_lines, line_number)
        # TODO: remove this and correctly do stuff
        if fnc.get("err") == "not-found":
            print("Could not find function when attemping autocomplete!")
            return []
        global_scope_vars = hierachy2.get_global_scope_variables(split_lines)
        global_scope_funcs = hierachy2.get_global_scope_functions(split_lines)
        local_scope_vars = hierachy2.get_local_scope_variables(split_lines, line_number)
        
        class_name = None
        
        # WTF! why do I have to add/subtract some random offset from line_number for each new feature
        # for lineyas,asyline in enumerate(split_lines):
        #     print(f"{lineyas}: {asyline}")
        # print(f"line_number+2 = {line_number+2}, len(codelines) = {len(split_lines)}")
        # print(f"line: {line}# prev line: {split_lines[line_number - 2]}#")
        # t = line.strip().split(".")
        t = line.split(".")
        # print(t)
        # print(f"len(t) == {len(t)}")

        if len(t) >= 2 and line.strip().endswith("."):
            if fnc.get("err") != "not-found":
                fnc["arguments"] = hierachy2.remove_handles_from_args(fnc["arguments"])
                for x in fnc["arguments"]:
                    if x["name"].lower() == hierachy2.remove_handle(t[0].split("[")[0]).lower():
                        class_name = x["type"]
                        break

            for x in global_scope_vars + local_scope_vars:
                if x["name"].lower() == hierachy2.remove_handle(t[0].split("[")[0]).lower():
                    class_name = x["type"]
                    break
            for p in globalProperties.values():
                for prop in p:
                    if prop["name"].lower() == hierachy2.remove_handle(t[0].split("[")[0]):
                        ret, attrs = hierachy2.remove_reserved_keywords(prop["full"])
                        class_name = hierachy2.remove_handle(ret)
                        break

        if class_name == None:
            class_name = line.strip()
            # NO CLUE
            s = split_lines[line_number - 2].strip().split("// @force-autocomplete-class ")

            if len(s) >= 2:
                class_name = s[1]
            else:
                class_name = None
        
        def handle_prop(prop):
            if prop["type"] == "function" or prop["type"] == "event":
                suggestions.append({
                    "type": prop["type"],
                    "name": prop["name"],
                    "description": prop["description"],
                    "full": prop["full"],
                    "items": prop["arguments"]
                })
            else:
                suggestions.append({
                    "type": prop["type"],
                    "name": prop["name"],
                    "description": prop["description"]
                })
        
        # enum identification
        # TODO allow enum identification for this file
        enum = split_lines[line_number - 1][:char].strip().split(" ")[-1].split("::")[0].lower()
        enums_found = False

        # print(f"BFR enum: |{enum}| line[:char]: |{split_lines[line_number - 1][:char]}|")
        # st=split_lines[line_number - 1][:char].strip().split(" ")
        # print(f"line: |{split_lines[line_number - 1]}|, char: |{char}| split: |{st}|")
        # print(f"next line: |{split_lines[line_number - 1]}|")
        # for ie, ieline in enumerate(split_lines):
        #     print(f"{ie}: {ieline}")
        
        if class_name == None:
            for enumStarterX in ENUM_ARRAY:
                enumStarter = enumStarterX.split("::")[0].lower()

                if len(split_lines[line_number - 1].strip().split("// @force-autocomplete-enum ")) >= 2:
                    enum = split_lines[line_number - 1].strip().split("// @force-autocomplete-enum ")[1] + "::"
                    enum = enum.lower()
                
                if enum.startswith(enumStarter):
                    enums_found = True
                    
                    for enumName in ENUM_ARRAY[enumStarterX]:
                        suggestions.append({
                            "type": "property",
                            "name": enumName,
                            "description": enumStarterX
                        })
                    break

            if enums_found:
                return suggestions

        if class_name != None:
            if classProperties.get(class_name):
                for prop in classProperties[class_name]:
                    handle_prop(prop)
        else:
            can_autocomplete_namespace = False
            namespaces = hierachy2.get_namespaces(split_lines)

            for namespace in namespaces:
                if line.split("::")[0] == namespace["name"].lower():
                    for prop in namespace["vars"] + namespace["funs"]:
                        handle_prop({**prop, **{"description": ""}})
                    can_autocomplete_namespace = True
                    break

            for namespace in namespaces:
                if line.split("::")[0] != namespace["name"].lower() and not can_autocomplete_namespace:
                    suggestions.append({
                        "type": "namespace",
                        "name": namespace["name"],
                        "description": ""
                    })

            if can_autocomplete_namespace:
                return suggestions

            for prop in fnc["arguments"]:
                suggestions.append({
                    "type": prop["type"],
                    "name": prop["name"],
                    "description": ""
                })
        
            for fn in global_scope_funcs:
                if fn["name"] in eventsList:
                    continue
                handle_prop(fn)

            for scope_vars in [global_scope_vars, local_scope_vars]:
                for p in scope_vars:
                    description = split_lines[p["line"] - 1].split("//", 1)[-1].strip() if split_lines[p["line"] - 1].strip().startswith("//") else ""
                    generated_p = {"type": p["type"], "name": p["name"], "description": description}
                    handle_prop(generated_p)

            for props in globalProperties.values():
                for prop in props:
                    handle_prop(prop)
        
        return suggestions

import socket, threading, time
import signal

clis = []

def sigint_handler(signum, frame):
    print("SIGINT received. Closing the server.")
    for cli in clis:
        cli.close()
    exit()    

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.bind(("localhost", 17338))
sock.listen()
sock.settimeout(1)

signal.signal(signal.SIGINT, sigint_handler)

print("Now listening for clients at port 17338!")

import cProfile, pstats

def handle_client(conn: socket.socket, addr: Tuple[str, int]):
    profiler = cProfile.Profile()

    while True:
        try:
            data = conn.recv(4096)
            if not data:
                break
        except ConnectionError or ConnectionAbortedError or ConnectionRefusedError or ConnectionResetError:
            print("Client disconnected!")
            break

        try:
            data = json.loads(data)
        except json.decoder.JSONDecodeError as e:
            if e.msg.startswith("Extra data:"):
                print("extra data found!")
                print("getting last item and autocompleting using it!")
                data = json.loads(data.split(b"{")[-1])
            else:
                continue

        startTime = time.time()

        linter = JJ2PlusLinter(data["file"])
        autocomplete_data = linter.autocomplete(data["line"], data["char"])

        # result, profiling_data = profiler.runctx("linter.autocomplete(data["line"], data["char"])", globals(), locals()), None
        # stats = pstats.Stats(profiler)
        # stats.strip_dirs()
        # stats.sort_stats('time')
        # stats.print_stats()
        # print(result)

        endTime = time.time()

        print(f"Took {endTime - startTime} to analyze with {len(autocomplete_data)} elements!") # type: ignore
        
        autocomplete_bytes = json.dumps(autocomplete_data).encode("utf-8")
        linter_bytes = json.dumps(linter.lint(data["type"] == "ADVANCED_AUTOCOMPLETE" == 1)).encode("utf-8")

        conn.sendall(autocomplete_bytes + b"\n" + linter_bytes)

while True:
    try:
        conn, addr = sock.accept()

        threading.Thread(target=handle_client, args=(conn, addr)).start()
        clis.append(conn)
    except socket.timeout:
        pass
