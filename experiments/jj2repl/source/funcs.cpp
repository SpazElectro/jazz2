#include "funcs.h"

int Add(int a, int b) {
    return a + b;
}

double AddD(double a, double b) {
    return a + b;
}

int Add3(int a, int b, int c) {
    return a + b + c;
}

void RegisterFunctions(asIScriptEngine* engine) {
    engine->RegisterGlobalFunction("int Add(int, int)", asFUNCTION(Add), asCALL_CDECL);
    engine->RegisterGlobalFunction("double Add(double, double)", asFUNCTION(AddD), asCALL_CDECL);
    engine->RegisterGlobalFunction("int Add(int, int, int)", asFUNCTION(Add3), asCALL_CDECL);
}
