import json

OUTPUT = """// This file was automatically generated. Do not modify.
#include "funcs.h"

void RegisterFunctions(asIScriptEngine* engine) {
%REGISTER_FUNCTIONS%
}

%DEFINE_FUNCTIONS%
"""

f = open("global.json").read()
f = json.loads(f)

define_functions = ""
register_functions = ""

# actual codegen goes here

funcs = f["globalfunctionsList"]
for x in funcs:
    returns = "0"
    x["full"] = x["full"].replace("@", "*")
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
        returns = "new "+(return_type.replace("*", ""))+"()"
    
    define_functions += (x["full"].replace("&in", "&").replace("& in", "&")) + " {\n"
    define_functions += f"\treturn {returns};\n"
    define_functions += "}\n\n"

    # engine->RegisterGlobalFunction("int Add(int, int)", asFUNCTION(Add), asCALL_CDECL);

    register_functions += f"\tengine->RegisterGlobalFunction(\"{x['full']}\", asFUNCTION({x['name']}), asCALL_CDECL);\n"
# codegen end

OUTPUT = OUTPUT.replace("%DEFINE_FUNCTIONS%", define_functions).replace(
    "%REGISTER_FUNCTIONS%", register_functions
)
open("output.cpp", "w").write(OUTPUT)
