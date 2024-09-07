import json

OUTPUT = """// This file was automatically generated. Do not modify.
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

%DEFINE_FUNCTIONS%

void RegisterFunctions(asIScriptEngine* engine) {
%REGISTER_FUNCTIONS%
}
"""

f = open("global.json").read()
f = json.loads(f)

define_functions = ""
register_functions = ""

# actual codegen goes here

funcs = f["globalfunctionsList"]
for x in funcs:
    returns = "0"
    return_type = x["full"].split(x["name"])[0].strip()
    if return_type == "bool":
        returns = "true"
    elif return_type == "void":
        returns = ""
    elif return_type == "float":
        returns = "0.0"
    elif return_type == "string":
        returns = "\"\""
    elif return_type != "int" and return_type != "uint16" and return_type != "uint64" and return_type != "uint":
        print(f"IDK what this is {return_type}")
        
    define_functions += x["full"] + " {\n"
    define_functions += f"\treturn {returns};\n"
    define_functions += "}\n\n"

    # engine->RegisterGlobalFunction("int Add(int, int)", asFUNCTION(Add), asCALL_CDECL);

    register_functions += f"\tengine->RegisterGlobalFunction(\"{x['full']}\", asFUNCTION({x['name']}), asCALL_CDECL);\n"
# codegen end

OUTPUT = OUTPUT.replace("%DEFINE_FUNCTIONS%", define_functions).replace(
    "%REGISTER_FUNCTIONS%", register_functions
)
open("output.cpp", "w").write(OUTPUT)
