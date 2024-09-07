#include <angelscript.h>
#include <iostream>
#include <fstream>
#include "funcs.h"
#include "runtime.h"

// TODO don't hardcode these
#include "G:/steve/angelscript_2.37.0/add_on/scriptarray/scriptarray.h"
#include "G:/steve/angelscript_2.37.0/add_on/scriptstdstring/scriptstdstring.h"
#include "G:/steve/angelscript_2.37.0/add_on/scriptmath/scriptmath.h"
#include "G:/steve/angelscript_2.37.0/add_on/scriptdictionary/scriptdictionary.h"
#include "G:/steve/angelscript_2.37.0/add_on/scriptany/scriptany.h"
#include "G:/steve/angelscript_2.37.0/add_on/scriptmath/scriptmathcomplex.h"

static bool hasErrored = false;

void asMessageCallback(const asSMessageInfo* msg, void* param) {
    std::cerr << "AngelScript error: " << msg->message << std::endl;
    hasErrored = true;
}

std::string readScriptFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open script file: " << filename << std::endl;
        return "";
    }
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

void asPrint(const std::string& message) {
    std::cout << message << std::endl;
}

int main() {
    asIScriptEngine* engine = asCreateScriptEngine();
    if (!engine) {
        std::cerr << "Failed to create AngelScript engine." << std::endl;
        return -1;
    }

    engine->SetMessageCallback(asFUNCTION(asMessageCallback), nullptr, asCALL_CDECL);

    RegisterScriptArray(engine, true);
    RegisterStdString(engine);
    RegisterStdStringUtils(engine);
    RegisterScriptDictionary(engine);
    RegisterScriptMath(engine);
    RegisterScriptMathComplex(engine);
    RegisterScriptAny(engine);
    
    engine->RegisterGlobalFunction("void print(const string& in)", asFUNCTION(asPrint), asCALL_CDECL);

    RegisterFunctions(engine);

    std::string script = readScriptFromFile("script.as");
    if (script.empty()) {
        engine->ShutDownAndRelease();
        return -1;
    }

    asIScriptModule* md = engine->GetModule("module", asGM_ALWAYS_CREATE);
    md->AddScriptSection("script", script.c_str());
    md->Build();

    if(hasErrored) {
        engine->ShutDownAndRelease();
        return 0;
    }

    RunRuntime(engine, md);

    engine->ShutDownAndRelease();

    return 0;
}
