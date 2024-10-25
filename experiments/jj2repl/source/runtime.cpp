#include "runtime.h"

void RunRuntime(asIScriptEngine* engine, asIScriptModule* md) {
    asIScriptContext* ctx = engine->CreateContext();
    asIScriptFunction* func = md->GetFunctionByName("main");

    if (func) {
        ctx->Prepare(func);
        ctx->Execute();
    } else {
        std::cerr << "Function 'main' not found in script!" << std::endl;
    }

    ctx->Release();
}
