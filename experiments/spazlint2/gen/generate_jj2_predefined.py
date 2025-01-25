import datetime
import json
from typing import TypedDict
from typing import List
from pprint import pprint

# if the first argument is a string, print it
# else, use pprint
oprint = print
print = lambda *args, **kwargs: oprint(*args, **kwargs) if isinstance(args[0], str) else pprint(*args, **kwargs)

type APIDumpEnums = dict[str, List[str]]
class APIDumpArgument(TypedDict):
    type: str
    name: str
    attributes: List[dict]
class APIDumpItem(TypedDict):
    name: str
    description: str
    full: str
    type: str
    arguments: List[APIDumpArgument]
    attributes: List[dict]
class APIDumps(TypedDict):
    eventsList: List[APIDumpItem]
    globalpropertiesList: List[APIDumpItem]
    globalfunctionsList: List[APIDumpItem]
    enums: APIDumpEnums

def has_attribute(attributes: List[dict], name: str) -> bool:
    for attr in attributes:
        if attr["type"] == name:
            return True
    return False

def process_full(full: str, attributes: List[dict]) -> str:
    if has_attribute(attributes, "returns_array"):
        rtype = full.split(" ")[0]
        if rtype == "const":
            rtype = full.split(" ")[1]
        # rtype somevar[] -> array<rtype> somevar
        full = full.replace(f"{rtype} ", f"array<{rtype}> ")
        full = full.split("[")[0]
    full = full.replace(" &in", "").replace(" &out", "")
    full = full.replace("jjBEHAVIOR", "BEHAVIOR::Behavior")
    return full + ";"

api_dump = open("jazzangel.json").read()
api: APIDumps = json.loads(api_dump)
output = f"// This file was automatically generated at {datetime.datetime.now()}\n\n#include 'jj2_additional.predefined' // REMOVE_LINE\n\n"

enums = api["enums"]
proper_enums = {}
for enum, keys in enums.items():
    namespace, enum_name = enum.split("::")
    if namespace not in proper_enums:
        proper_enums[namespace] = {}
    proper_enums[namespace][enum_name] = keys

for namespace, enums in proper_enums.items():
    output += f"namespace {namespace} {{\n"
    for enum_name, keys in enums.items():
        output += f"    enum {enum_name} {{\n"
        for key in keys:
            output += f"        {key},\n"
        output += f"    }};\n"
    output += f"    // Aliases\n"
    defined = []
    for enum_name, keys in enums.items():
        for key in keys:
            if key in defined:
                continue
            defined.append(key)
            output += f"    const auto {key} = {enum_name}::{key};\n"
    output += f"}};\n"

for dump in api.keys():
    if dump == "enums":
        continue
    if dump == "eventsList" or dump == "globalpropertiesList" or dump == "globalfunctionsList":
        continue
    
    output += f"\nclass {dump.split('List')[0]} {{\n"
    ddump = []
    for itm in api[dump]:
        itm: APIDumpItem = itm
        full = process_full(itm["full"], itm["attributes"])

        # stupid hacks
        if dump == "jjLAYERList" or dump == "jjPARTICLEList":
            # if the property is some weird type just make it an int
            if len(full.split()) == 1:
                full = f"int {full} // (unknown type)"
        if dump == "jjSTREAMList":
            # if the itm name is "push" or "pop", inject different push methods because of T
            if itm["name"] == "push":
                full = "void push(int value);\n"
                full += "\tvoid push(float value);\n"
                full += "\tvoid push(string& value);"
            if itm["name"] == "pop":
                full = "int pop();\n"
                full += "\tfloat pop();\n"
                full += "\tstring pop();"
        
        if full in ddump:
            continue
        ddump.append(full)
        output += f"    {full}\n"
    
    # more stupid hacks
    if dump == "jjPALCOLORList":
        output += f"\tjjPALCOLOR(int r, int g, int b);\n"

    output += f"}};\n"

defined = []
for dump in api.keys():
    if dump == "eventsList" or dump == "globalpropertiesList" or dump == "globalfunctionsList":
        for itm in api[dump]:
            full = process_full(itm["full"], itm["attributes"])
            if full in defined:
                continue
            defined.append(full)
            output += full + "\n"

with open("predefined/jj2.predefined", "w") as f:
    f.write(output)
    f.close()

print(f"Wrote {len(output.splitlines())} lines to jj2.predefined")
