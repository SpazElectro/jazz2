// This file was automatically generated. Do not modify.
#include "funcs.h"

// Typedefs
typedef uint8_t  byte;
typedef int8_t   int8;
typedef uint8_t  uint8;

typedef int16_t  int16;
typedef uint16_t uint16;

typedef int32_t  int;
typedef uint32_t uint;
typedef float    float32;

typedef int64_t  int64;
typedef uint64_t uint64;
typedef double   double64;

int jjAddObject(uint8 eventID, float xPixel, float yPixel, uint16 creatorID = 0, CREATOR::Type creatorType = CREATOR::OBJECT, jjBEHAVIOR behavior = BEHAVIOR::DEFAULT) {
	return 0;
}

jjPARTICLE@ jjAddParticle(PARTICLE::Type type) {
	return 0;
}

void jjAddParticlePixelExplosion(float xPixel, float yPixel, int curFrame, int direction, int mode) {
	return ;
}

void jjAddParticleTileExplosion(uint16 xTile, uint16 yTile, uint16 tile, bool collapseSceneryStyle) {
	return ;
}

void jjAlert(const string &in text, bool sendToAll = false, STRING::Size size = STRING::SMALL) {
	return ;
}

void jjChat(const string &in text, bool teamchat = false) {
	return ;
}

void jjConsole(const string &in text, bool sendToAll = false) {
	return ;
}

float jjCos(uint angle) {
	return 0.0;
}

uint jjCRC32(const jjSTREAM &in input, uint crc = 0) {
	return 0;
}

void jjDebug(const string &in text, bool timestamp = false) {
	return ;
}

void jjDeleteObject(int objectID) {
	return ;
}

void jjDrawPixel(float xPixel, float yPixel, uint8 color, SPRITE::Mode mode = SPRITE::NORMAL, int param = 0, int8 layerZ = 4, uint8 layerXY = 4, int8 playerID = -1) {
	return ;
}

void jjDrawRectangle(float xPixel, float yPixel, int width, int height, uint8 color, SPRITE::Mode mode = SPRITE::NORMAL, int param = 0, int8 layerZ = 4, uint8 layerXY = 4, int8 playerID = -1) {
	return ;
}

void jjDrawResizedSprite(float xPixel, float yPixel, int setID, uint8 animation, uint8 frame, float xScale, float yScale, SPRITE::Mode mode = SPRITE::NORMAL, int param = 0, int8 layerZ = 4, uint8 layerXY = 4, int8 playerID = -1) {
	return ;
}

void jjDrawResizedSpriteFromCurFrame(float xPixel, float yPixel, uint sprite, float xScale, float yScale, SPRITE::Mode mode = SPRITE::NORMAL, int param = 0, int8 layerZ = 4, uint8 layerXY = 4, int8 playerID = -1) {
	return ;
}

void jjDrawRotatedSprite(float xPixel, float yPixel, int setID, uint8 animation, uint8 frame, int angle, float xScale = 1, float yScale = 1, SPRITE::Mode mode = SPRITE::NORMAL, int param = 0, int8 layerZ = 4, uint8 layerXY = 4, int8 playerID = -1) {
	return ;
}

void jjDrawRotatedSpriteFromCurFrame(float xPixel, float yPixel, uint sprite, int angle, float xScale = 1, float yScale = 1, SPRITE::Mode mode = SPRITE::NORMAL, int param = 0, int8 layerZ = 4, uint8 layerXY = 4, int8 playerID = -1) {
	return ;
}

void jjDrawSprite(float xPixel, float yPixel, int setID, uint8 animation, uint8 frame, int direction = 0, SPRITE::Mode mode = SPRITE::NORMAL, int param = 0, int8 layerZ = 4, uint8 layerXY = 4, int8 playerID = -1) {
	return ;
}

void jjDrawSpriteFromCurFrame(float xPixel, float yPixel, uint sprite, int direction = 0, SPRITE::Mode mode = SPRITE::NORMAL, int param = 0, int8 layerZ = 4, uint8 layerXY = 4, int8 playerID = -1) {
	return ;
}

void jjDrawString(float xPixel, float yPixel, const string &in text, STRING::SIZE size = STRING::SMALL, STRING::Mode mode = STRING::NORMAL, uint8 param = 0, int8 layerZ = 4, uint8 layerXY = 4, int8 playerID = -1) {
	return ;
}

void jjDrawString(float xPixel, float yPixel, const string &in text, const jjANIMATION &in animation, STRING::Mode mode = STRING::NORMAL, uint8 param = 0, int8 layerZ = 4, uint8 layerXY = 4, int8 playerID = -1) {
	return ;
}

void jjDrawString(float xPixel, float yPixel, const string &in text, STRING::SIZE size, const jjTEXTAPPEARANCE &in appearance, uint8 param1 = 0, SPRITE::Mode spriteMode = SPRITE::PALSHIFT, uint8 param2 = 0, int8 layerZ = 4, uint8 layerXY = 4, int8 playerID = -1) {
	return ;
}

void jjDrawString(float xPixel, float yPixel, const string &in text, const jjANIMATION &in animation, const jjTEXTAPPEARANCE &in appearance, uint8 param1 = 0, SPRITE::Mode spriteMode = SPRITE::PALSHIFT, uint8 param2 = 0, int8 layerZ = 4, uint8 layerXY = 4, int8 playerID = -1) {
	return ;
}

void jjDrawSwingingVineSpriteFromCurFrame(float xPixel, float yPixel, uint sprite, int length, int curvature, SPRITE::Mode mode = SPRITE::NORMAL, int param = 0, int8 layerZ = 4, uint8 layerXY = 4, int8 playerID = -1) {
	return ;
}

void jjDrawTile(float xPixel, float yPixel, uint16 tile, TILE::Quadrant tileQuadrant = TILE::ALLQUADRANTS, int8 layerZ = 4, uint8 layerXY = 4, int8 playerID = -1) {
	return ;
}

void jjEnableEachASFunction() {
	return ;
}

int jjEventGet(uint16 xTile, uint16 yTile) {
	return 0;
}

void jjEventSet(uint16 xTile, uint16 yTile, uint8 newEventID) {
	return ;
}

void jjEventSet(uint16 xTile, uint16 yTile, OBJECT::Object newEventID) {
	return ;
}

void jjEventSet(uint16 xTile, uint16 yTile, AREA::Area newEventID) {
	return ;
}

void jjGenerateSettableTileArea(uint8 layer, int xTile, int yTile, int width, int height) {
	return ;
}

jjPALCOLOR jjGetFadeColors() {
	return 0;
}

int jjGetModOrder() {
	return 0;
}

int jjGetModRow() {
	return 0;
}

int jjGetModSpeed() {
	return 0;
}

int jjGetModTempo() {
	return 0;
}

jjPUBLICINTERFACE@ jjGetPublicInterface(const string &in moduleName) {
	return 0;
}

uint16 jjGetStaticTile(uint16 tileID) {
	return 0;
}

int jjGetStringWidth(const string &in text, STRING::Size size, const jjTEXTAPPEARANCE &in style) {
	return 0;
}

int jjGetStringWidth(const string &in text, const jjANIMATION &in animation, const jjTEXTAPPEARANCE &in style) {
	return 0;
}

bool jjIsValidCheat(const string &in text) {
	return true;
}

void jjKillObject(int objectID) {
	return ;
}

array<jjLAYER@>@ jjLayerOrderGet() {
	return 0;
}

bool jjLayerOrderSet(const array<jjLAYER@>& in order) {
	return true;
}

array<jjLAYER@>@ jjLayersFromLevel(const string &in filename, const array<uint> &in layerIDs, int tileIDAdjustmentFactor = 0) {
	return 0;
}

bool jjMaskedHLine(int xPixel, int lineLength, int yPixel) {
	return true;
}

bool jjMaskedPixel(int xPixel, int yPixel) {
	return true;
}

int jjMaskedTopVLine(int xPixel, int yPixel, int lineLength) {
	return 0;
}

bool jjMaskedVLine(int xPixel, int yPixel, int lineLength) {
	return true;
}

bool jjMaskedHLine(int xPixel, int lineLength, int yPixel, uint8 layer) {
	return true;
}

bool jjMaskedPixel(int xPixel, int yPixel, uint8 layer) {
	return true;
}

int jjMaskedTopVLine(int xPixel, int yPixel, int lineLength, uint8 layer) {
	return 0;
}

bool jjMaskedVLine(int xPixel, int yPixel, int lineLength, uint8 layer) {
	return true;
}

bool jjMusicLoad(string filename, bool forceReload = false, bool temporary = false) {
	return true;
}

void jjMusicPause() {
	return ;
}

void jjMusicPlay() {
	return ;
}

void jjMusicResume() {
	return ;
}

void jjMusicStop() {
	return ;
}

void jjNxt(bool warp = false, bool fast = false) {
	return ;
}

void jjNxt(const string& filename = "", bool warp = false, bool fast = false) {
	return ;
}

int jjParameterGet(uint16 xTile, uint16 yTile, int8 offset, int8 length) {
	return 0;
}

void jjParameterSet(uint16 xTile, uint16 yTile, int8 offset, int8 length, int newValue) {
	return ;
}

array<jjPLAYER@>@ jjPlayersWithClientID(int clientID) {
	return 0;
}

void jjPrint(const string &in text, bool timestamp = false) {
	return ;
}

uint jjRandom() {
	return 0;
}

bool jjRegexIsValid(const string &in expression) {
	return true;
}

bool jjRegexMatch(const string &in text, const string &in expression, bool ignoreCase = false) {
	return true;
}

bool jjRegexMatch(const string &in text, const string &in expression, array<string> &out results, bool ignoreCase = false) {
	return true;
}

string jjRegexReplace(const string &in text, const string &in expression, const string &in replacement, bool ignoreCase = false) {
	return "";
}

bool jjRegexSearch(const string &in text, const string &in expression, bool ignoreCase = false) {
	return true;
}

bool jjRegexSearch(const string &in text, const string &in expression, array<string> &out results, bool ignoreCase = false) {
	return true;
}

void jjResetWaterGradient() {
	return ;
}

void jjSample(float xPixel, float yPixel, SOUND::Sample sample, int volume = 63, int frequency = 0) {
	return ;
}

bool jjSampleIsLoaded(SOUND::Sample sample) {
	return true;
}

bool jjSampleLoad(SOUND::Sample sample, string &in filename) {
	return true;
}

int jjSampleLooped(float xPixel, float yPixel, SOUND::Sample sample, int channel, int volume = 63, int frequency = 0) {
	return 0;
}

void jjSamplePriority(SOUND::Sample sample) {
	return ;
}

bool jjSendPacket(jjSTREAM &in packet, int toClientID = 0, uint toScriptModuleID = jjScriptModuleID) {
	return true;
}

void jjSetDarknessColor(jjPALCOLOR color = jjPALCOLOR(0, 0, 0)) {
	return ;
}

void jjSetFadeColors(uint8 red, uint8 green, uint8 blue) {
	return ;
}

void jjSetFadeColors(jjPALCOLOR color) {
	return ;
}

void jjSetFadeColors(uint8 paletteColorID = 207) {
	return ;
}

void jjSetLayerXSpeed(uint8 layerID, float newspeed, bool newSpeedIsAnAutoSpeed) {
	return ;
}

void jjSetLayerYSpeed(uint8 layerID, float newspeed, bool newSpeedIsAnAutoSpeed) {
	return ;
}

void jjSetModPosition(int order, int row, bool reset) {
	return ;
}

void jjSetModSpeed(uint8 speed) {
	return ;
}

void jjSetModTempo(uint8 tempo) {
	return ;
}

void jjSetWaterGradient(uint8 red1, uint8 green1, uint8 blue1, uint8 red2, uint8 green2, uint8 blue2) {
	return ;
}

void jjSetWaterGradient(jjPALCOLOR color1, jjPALCOLOR color2) {
	return ;
}

void jjSetWaterGradient() {
	return ;
}

void jjSetWaterLevel(float yPixel, bool instant) {
	return ;
}

float jjSin(uint angle) {
	return 0.0;
}

void jjSlideModChannelVolume(int channel, float volume, int milliseconds) {
	return ;
}

int jjSpriteModeFirstFreeMapping() {
	return 0;
}

jjPAL@ jjSpriteModeGetColorMapping(uint8 index) {
	return 0;
}

array<uint8>@ jjSpriteModeGetIndexMapping(uint8 index) {
	return 0;
}

bool jjSpriteModeIsMappingUsed(uint8 index) {
	return true;
}

void jjSpriteModeSetMapping(uint8 index, const array<uint8> &in indexMapping, const jjPAL &in rgbMapping) {
	return ;
}

void jjSpriteModeSetMapping(uint8 index, const array<uint8> &in indexMapping) {
	return ;
}

void jjSpy(const string &in text) {
	return ;
}

bool jjSwitchTrigger(uint8 id) {
	return true;
}

bool jjTakeScreenshot(const string &in filename = "") {
	return true;
}

uint16 jjTileGet(uint8 layer, int xTile, int yTile) {
	return 0;
}

uint16 jjTileSet(uint8 layer, int xTile, int yTile, uint16 newTile) {
	return 0;
}

bool jjTilesFromTileset(const string &in filename, uint firstTileID, uint tileCount, const array<uint8>@ paletteColorMapping = null) {
	return true;
}

void jjTriggerRock(uint8 id) {
	return ;
}

uint64 jjUnixTimeMs() {
	return 0;
}

uint64 jjUnixTimeSec() {
	return 0;
}

void jjUpdateTexturedBG() {
	return ;
}

bool jjZlibCompress(const jjSTREAM &in input, jjSTREAM &out output) {
	return true;
}

bool jjZlibUncompress(const jjSTREAM &in input, jjSTREAM &out output, uint size) {
	return true;
}



void RegisterFunctions(asIScriptEngine* engine) {
	engine->RegisterGlobalFunction("int jjAddObject(uint8 eventID, float xPixel, float yPixel, uint16 creatorID = 0, CREATOR::Type creatorType = CREATOR::OBJECT, jjBEHAVIOR behavior = BEHAVIOR::DEFAULT)", asFUNCTION(jjAddObject), asCALL_CDECL);
	engine->RegisterGlobalFunction("jjPARTICLE@ jjAddParticle(PARTICLE::Type type)", asFUNCTION(jjAddParticle), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjAddParticlePixelExplosion(float xPixel, float yPixel, int curFrame, int direction, int mode)", asFUNCTION(jjAddParticlePixelExplosion), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjAddParticleTileExplosion(uint16 xTile, uint16 yTile, uint16 tile, bool collapseSceneryStyle)", asFUNCTION(jjAddParticleTileExplosion), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjAlert(const string &in text, bool sendToAll = false, STRING::Size size = STRING::SMALL)", asFUNCTION(jjAlert), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjChat(const string &in text, bool teamchat = false)", asFUNCTION(jjChat), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjConsole(const string &in text, bool sendToAll = false)", asFUNCTION(jjConsole), asCALL_CDECL);
	engine->RegisterGlobalFunction("float jjCos(uint angle)", asFUNCTION(jjCos), asCALL_CDECL);
	engine->RegisterGlobalFunction("uint jjCRC32(const jjSTREAM &in input, uint crc = 0)", asFUNCTION(jjCRC32), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjDebug(const string &in text, bool timestamp = false)", asFUNCTION(jjDebug), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjDeleteObject(int objectID)", asFUNCTION(jjDeleteObject), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjDrawPixel(float xPixel, float yPixel, uint8 color, SPRITE::Mode mode = SPRITE::NORMAL, int param = 0, int8 layerZ = 4, uint8 layerXY = 4, int8 playerID = -1)", asFUNCTION(jjDrawPixel), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjDrawRectangle(float xPixel, float yPixel, int width, int height, uint8 color, SPRITE::Mode mode = SPRITE::NORMAL, int param = 0, int8 layerZ = 4, uint8 layerXY = 4, int8 playerID = -1)", asFUNCTION(jjDrawRectangle), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjDrawResizedSprite(float xPixel, float yPixel, int setID, uint8 animation, uint8 frame, float xScale, float yScale, SPRITE::Mode mode = SPRITE::NORMAL, int param = 0, int8 layerZ = 4, uint8 layerXY = 4, int8 playerID = -1)", asFUNCTION(jjDrawResizedSprite), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjDrawResizedSpriteFromCurFrame(float xPixel, float yPixel, uint sprite, float xScale, float yScale, SPRITE::Mode mode = SPRITE::NORMAL, int param = 0, int8 layerZ = 4, uint8 layerXY = 4, int8 playerID = -1)", asFUNCTION(jjDrawResizedSpriteFromCurFrame), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjDrawRotatedSprite(float xPixel, float yPixel, int setID, uint8 animation, uint8 frame, int angle, float xScale = 1, float yScale = 1, SPRITE::Mode mode = SPRITE::NORMAL, int param = 0, int8 layerZ = 4, uint8 layerXY = 4, int8 playerID = -1)", asFUNCTION(jjDrawRotatedSprite), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjDrawRotatedSpriteFromCurFrame(float xPixel, float yPixel, uint sprite, int angle, float xScale = 1, float yScale = 1, SPRITE::Mode mode = SPRITE::NORMAL, int param = 0, int8 layerZ = 4, uint8 layerXY = 4, int8 playerID = -1)", asFUNCTION(jjDrawRotatedSpriteFromCurFrame), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjDrawSprite(float xPixel, float yPixel, int setID, uint8 animation, uint8 frame, int direction = 0, SPRITE::Mode mode = SPRITE::NORMAL, int param = 0, int8 layerZ = 4, uint8 layerXY = 4, int8 playerID = -1)", asFUNCTION(jjDrawSprite), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjDrawSpriteFromCurFrame(float xPixel, float yPixel, uint sprite, int direction = 0, SPRITE::Mode mode = SPRITE::NORMAL, int param = 0, int8 layerZ = 4, uint8 layerXY = 4, int8 playerID = -1)", asFUNCTION(jjDrawSpriteFromCurFrame), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjDrawString(float xPixel, float yPixel, const string &in text, STRING::SIZE size = STRING::SMALL, STRING::Mode mode = STRING::NORMAL, uint8 param = 0, int8 layerZ = 4, uint8 layerXY = 4, int8 playerID = -1)", asFUNCTION(jjDrawString), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjDrawString(float xPixel, float yPixel, const string &in text, const jjANIMATION &in animation, STRING::Mode mode = STRING::NORMAL, uint8 param = 0, int8 layerZ = 4, uint8 layerXY = 4, int8 playerID = -1)", asFUNCTION(jjDrawString), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjDrawString(float xPixel, float yPixel, const string &in text, STRING::SIZE size, const jjTEXTAPPEARANCE &in appearance, uint8 param1 = 0, SPRITE::Mode spriteMode = SPRITE::PALSHIFT, uint8 param2 = 0, int8 layerZ = 4, uint8 layerXY = 4, int8 playerID = -1)", asFUNCTION(jjDrawString), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjDrawString(float xPixel, float yPixel, const string &in text, const jjANIMATION &in animation, const jjTEXTAPPEARANCE &in appearance, uint8 param1 = 0, SPRITE::Mode spriteMode = SPRITE::PALSHIFT, uint8 param2 = 0, int8 layerZ = 4, uint8 layerXY = 4, int8 playerID = -1)", asFUNCTION(jjDrawString), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjDrawSwingingVineSpriteFromCurFrame(float xPixel, float yPixel, uint sprite, int length, int curvature, SPRITE::Mode mode = SPRITE::NORMAL, int param = 0, int8 layerZ = 4, uint8 layerXY = 4, int8 playerID = -1)", asFUNCTION(jjDrawSwingingVineSpriteFromCurFrame), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjDrawTile(float xPixel, float yPixel, uint16 tile, TILE::Quadrant tileQuadrant = TILE::ALLQUADRANTS, int8 layerZ = 4, uint8 layerXY = 4, int8 playerID = -1)", asFUNCTION(jjDrawTile), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjEnableEachASFunction()", asFUNCTION(jjEnableEachASFunction), asCALL_CDECL);
	engine->RegisterGlobalFunction("int jjEventGet(uint16 xTile, uint16 yTile)", asFUNCTION(jjEventGet), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjEventSet(uint16 xTile, uint16 yTile, uint8 newEventID)", asFUNCTION(jjEventSet), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjEventSet(uint16 xTile, uint16 yTile, OBJECT::Object newEventID)", asFUNCTION(jjEventSet), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjEventSet(uint16 xTile, uint16 yTile, AREA::Area newEventID)", asFUNCTION(jjEventSet), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjGenerateSettableTileArea(uint8 layer, int xTile, int yTile, int width, int height)", asFUNCTION(jjGenerateSettableTileArea), asCALL_CDECL);
	engine->RegisterGlobalFunction("jjPALCOLOR jjGetFadeColors()", asFUNCTION(jjGetFadeColors), asCALL_CDECL);
	engine->RegisterGlobalFunction("int jjGetModOrder()", asFUNCTION(jjGetModOrder), asCALL_CDECL);
	engine->RegisterGlobalFunction("int jjGetModRow()", asFUNCTION(jjGetModRow), asCALL_CDECL);
	engine->RegisterGlobalFunction("int jjGetModSpeed()", asFUNCTION(jjGetModSpeed), asCALL_CDECL);
	engine->RegisterGlobalFunction("int jjGetModTempo()", asFUNCTION(jjGetModTempo), asCALL_CDECL);
	engine->RegisterGlobalFunction("jjPUBLICINTERFACE@ jjGetPublicInterface(const string &in moduleName)", asFUNCTION(jjGetPublicInterface), asCALL_CDECL);
	engine->RegisterGlobalFunction("uint16 jjGetStaticTile(uint16 tileID)", asFUNCTION(jjGetStaticTile), asCALL_CDECL);
	engine->RegisterGlobalFunction("int jjGetStringWidth(const string &in text, STRING::Size size, const jjTEXTAPPEARANCE &in style)", asFUNCTION(jjGetStringWidth), asCALL_CDECL);
	engine->RegisterGlobalFunction("int jjGetStringWidth(const string &in text, const jjANIMATION &in animation, const jjTEXTAPPEARANCE &in style)", asFUNCTION(jjGetStringWidth), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool jjIsValidCheat(const string &in text)", asFUNCTION(jjIsValidCheat), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjKillObject(int objectID)", asFUNCTION(jjKillObject), asCALL_CDECL);
	engine->RegisterGlobalFunction("array<jjLAYER@>@ jjLayerOrderGet()", asFUNCTION(jjLayerOrderGet), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool jjLayerOrderSet(const array<jjLAYER@>& in order)", asFUNCTION(jjLayerOrderSet), asCALL_CDECL);
	engine->RegisterGlobalFunction("array<jjLAYER@>@ jjLayersFromLevel(const string &in filename, const array<uint> &in layerIDs, int tileIDAdjustmentFactor = 0)", asFUNCTION(jjLayersFromLevel), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool jjMaskedHLine(int xPixel, int lineLength, int yPixel)", asFUNCTION(jjMaskedHLine), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool jjMaskedPixel(int xPixel, int yPixel)", asFUNCTION(jjMaskedPixel), asCALL_CDECL);
	engine->RegisterGlobalFunction("int jjMaskedTopVLine(int xPixel, int yPixel, int lineLength)", asFUNCTION(jjMaskedTopVLine), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool jjMaskedVLine(int xPixel, int yPixel, int lineLength)", asFUNCTION(jjMaskedVLine), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool jjMaskedHLine(int xPixel, int lineLength, int yPixel, uint8 layer)", asFUNCTION(jjMaskedHLine), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool jjMaskedPixel(int xPixel, int yPixel, uint8 layer)", asFUNCTION(jjMaskedPixel), asCALL_CDECL);
	engine->RegisterGlobalFunction("int jjMaskedTopVLine(int xPixel, int yPixel, int lineLength, uint8 layer)", asFUNCTION(jjMaskedTopVLine), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool jjMaskedVLine(int xPixel, int yPixel, int lineLength, uint8 layer)", asFUNCTION(jjMaskedVLine), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool jjMusicLoad(string filename, bool forceReload = false, bool temporary = false)", asFUNCTION(jjMusicLoad), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjMusicPause()", asFUNCTION(jjMusicPause), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjMusicPlay()", asFUNCTION(jjMusicPlay), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjMusicResume()", asFUNCTION(jjMusicResume), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjMusicStop()", asFUNCTION(jjMusicStop), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjNxt(bool warp = false, bool fast = false)", asFUNCTION(jjNxt), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjNxt(const string& filename = "", bool warp = false, bool fast = false)", asFUNCTION(jjNxt), asCALL_CDECL);
	engine->RegisterGlobalFunction("int jjParameterGet(uint16 xTile, uint16 yTile, int8 offset, int8 length)", asFUNCTION(jjParameterGet), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjParameterSet(uint16 xTile, uint16 yTile, int8 offset, int8 length, int newValue)", asFUNCTION(jjParameterSet), asCALL_CDECL);
	engine->RegisterGlobalFunction("array<jjPLAYER@>@ jjPlayersWithClientID(int clientID)", asFUNCTION(jjPlayersWithClientID), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjPrint(const string &in text, bool timestamp = false)", asFUNCTION(jjPrint), asCALL_CDECL);
	engine->RegisterGlobalFunction("uint jjRandom()", asFUNCTION(jjRandom), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool jjRegexIsValid(const string &in expression)", asFUNCTION(jjRegexIsValid), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool jjRegexMatch(const string &in text, const string &in expression, bool ignoreCase = false)", asFUNCTION(jjRegexMatch), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool jjRegexMatch(const string &in text, const string &in expression, array<string> &out results, bool ignoreCase = false)", asFUNCTION(jjRegexMatch), asCALL_CDECL);
	engine->RegisterGlobalFunction("string jjRegexReplace(const string &in text, const string &in expression, const string &in replacement, bool ignoreCase = false)", asFUNCTION(jjRegexReplace), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool jjRegexSearch(const string &in text, const string &in expression, bool ignoreCase = false)", asFUNCTION(jjRegexSearch), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool jjRegexSearch(const string &in text, const string &in expression, array<string> &out results, bool ignoreCase = false)", asFUNCTION(jjRegexSearch), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjResetWaterGradient()", asFUNCTION(jjResetWaterGradient), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjSample(float xPixel, float yPixel, SOUND::Sample sample, int volume = 63, int frequency = 0)", asFUNCTION(jjSample), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool jjSampleIsLoaded(SOUND::Sample sample)", asFUNCTION(jjSampleIsLoaded), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool jjSampleLoad(SOUND::Sample sample, string &in filename)", asFUNCTION(jjSampleLoad), asCALL_CDECL);
	engine->RegisterGlobalFunction("int jjSampleLooped(float xPixel, float yPixel, SOUND::Sample sample, int channel, int volume = 63, int frequency = 0)", asFUNCTION(jjSampleLooped), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjSamplePriority(SOUND::Sample sample)", asFUNCTION(jjSamplePriority), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool jjSendPacket(jjSTREAM &in packet, int toClientID = 0, uint toScriptModuleID = jjScriptModuleID)", asFUNCTION(jjSendPacket), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjSetDarknessColor(jjPALCOLOR color = jjPALCOLOR(0, 0, 0))", asFUNCTION(jjSetDarknessColor), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjSetFadeColors(uint8 red, uint8 green, uint8 blue)", asFUNCTION(jjSetFadeColors), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjSetFadeColors(jjPALCOLOR color)", asFUNCTION(jjSetFadeColors), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjSetFadeColors(uint8 paletteColorID = 207)", asFUNCTION(jjSetFadeColors), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjSetLayerXSpeed(uint8 layerID, float newspeed, bool newSpeedIsAnAutoSpeed)", asFUNCTION(jjSetLayerXSpeed), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjSetLayerYSpeed(uint8 layerID, float newspeed, bool newSpeedIsAnAutoSpeed)", asFUNCTION(jjSetLayerYSpeed), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjSetModPosition(int order, int row, bool reset)", asFUNCTION(jjSetModPosition), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjSetModSpeed(uint8 speed)", asFUNCTION(jjSetModSpeed), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjSetModTempo(uint8 tempo)", asFUNCTION(jjSetModTempo), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjSetWaterGradient(uint8 red1, uint8 green1, uint8 blue1, uint8 red2, uint8 green2, uint8 blue2)", asFUNCTION(jjSetWaterGradient), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjSetWaterGradient(jjPALCOLOR color1, jjPALCOLOR color2)", asFUNCTION(jjSetWaterGradient), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjSetWaterGradient()", asFUNCTION(jjSetWaterGradient), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjSetWaterLevel(float yPixel, bool instant)", asFUNCTION(jjSetWaterLevel), asCALL_CDECL);
	engine->RegisterGlobalFunction("float jjSin(uint angle)", asFUNCTION(jjSin), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjSlideModChannelVolume(int channel, float volume, int milliseconds)", asFUNCTION(jjSlideModChannelVolume), asCALL_CDECL);
	engine->RegisterGlobalFunction("int jjSpriteModeFirstFreeMapping()", asFUNCTION(jjSpriteModeFirstFreeMapping), asCALL_CDECL);
	engine->RegisterGlobalFunction("jjPAL@ jjSpriteModeGetColorMapping(uint8 index)", asFUNCTION(jjSpriteModeGetColorMapping), asCALL_CDECL);
	engine->RegisterGlobalFunction("array<uint8>@ jjSpriteModeGetIndexMapping(uint8 index)", asFUNCTION(jjSpriteModeGetIndexMapping), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool jjSpriteModeIsMappingUsed(uint8 index)", asFUNCTION(jjSpriteModeIsMappingUsed), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjSpriteModeSetMapping(uint8 index, const array<uint8> &in indexMapping, const jjPAL &in rgbMapping)", asFUNCTION(jjSpriteModeSetMapping), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjSpriteModeSetMapping(uint8 index, const array<uint8> &in indexMapping)", asFUNCTION(jjSpriteModeSetMapping), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjSpy(const string &in text)", asFUNCTION(jjSpy), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool jjSwitchTrigger(uint8 id)", asFUNCTION(jjSwitchTrigger), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool jjTakeScreenshot(const string &in filename = "")", asFUNCTION(jjTakeScreenshot), asCALL_CDECL);
	engine->RegisterGlobalFunction("uint16 jjTileGet(uint8 layer, int xTile, int yTile)", asFUNCTION(jjTileGet), asCALL_CDECL);
	engine->RegisterGlobalFunction("uint16 jjTileSet(uint8 layer, int xTile, int yTile, uint16 newTile)", asFUNCTION(jjTileSet), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool jjTilesFromTileset(const string &in filename, uint firstTileID, uint tileCount, const array<uint8>@ paletteColorMapping = null)", asFUNCTION(jjTilesFromTileset), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjTriggerRock(uint8 id)", asFUNCTION(jjTriggerRock), asCALL_CDECL);
	engine->RegisterGlobalFunction("uint64 jjUnixTimeMs()", asFUNCTION(jjUnixTimeMs), asCALL_CDECL);
	engine->RegisterGlobalFunction("uint64 jjUnixTimeSec()", asFUNCTION(jjUnixTimeSec), asCALL_CDECL);
	engine->RegisterGlobalFunction("void jjUpdateTexturedBG()", asFUNCTION(jjUpdateTexturedBG), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool jjZlibCompress(const jjSTREAM &in input, jjSTREAM &out output)", asFUNCTION(jjZlibCompress), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool jjZlibUncompress(const jjSTREAM &in input, jjSTREAM &out output, uint size)", asFUNCTION(jjZlibUncompress), asCALL_CDECL);

}
