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
        # rtype somevar[length-or-indexing-type] -> fakearray<returns, indexing> somevar
        
        indextype = full.split("[")[1].split("]")[0]
        if indextype.startswith("0x"):
            indextype = "length"
        # if indextype can be parsed into an int, then this is length!
        try:
            int(indextype)
            indextype = "length"
        except ValueError:
            pass
        if indextype == "length":
            # should turn into array<jjOBJ@> jjObjects = array<int>(length)
            # instead of array<jjOBJ@> jjObjects(4096)
            full = full.replace(f"{rtype} ", f"array<{rtype}> ")
            full = full.split("[")[0] + f" = array<{rtype}>({full.split('[')[1].split(']')[0]})"
        else:
            othertype = ""
            prop = full.split("[")[0].split(" ")[1]
            # TODO don't hardcode this
            if prop == "powerup" or prop == "ammo":
                othertype = "uint"
            if othertype != "":
                othertype = f", {othertype}"
            full = full.replace(f"{rtype} ", f"fakearray<{indextype}, {rtype}{othertype}> ")
            full = full.split("[")[0]
        
    full = full.replace(" &in", "").replace(" &out", "")
    full = full.replace("jjBEHAVIOR", "BEHAVIOR::Behavior")
    return full + ";"

def better_description(description: str, tab_level: int) -> str:
    words = description.split()
    lines = []
    current_line = ""
    for word in words:
        if len(current_line) + len(word) + 1 > 80:
            lines.append(current_line.strip())
            current_line = word
        else:
            if current_line:
                current_line += " " + word
            else:
                current_line = word
    if current_line:
        lines.append(current_line.strip())
    return ("\n" + "\t" * tab_level).join(lines) + ("\t" * tab_level)

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
            if enum_name == "Set" and key == "CUSTOM":
                output += f"    const array<Set> CUSTOM;\n"
            else:
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
        desc = f"/*\n\t{better_description(itm['description'], 1)}\n\t*/\n\t"
        processed = process_full(itm["full"], itm["attributes"])
        full = desc + processed

        # stupid hacks
        if dump == "jjLAYERList" or dump == "jjPARTICLEList":
            # if the property is some weird type just make it an int
            if len(itm["full"].split()) == 1:
                full = f"{desc}int {processed} // (unknown type)"
        if dump == "jjSTREAMList":
            if itm["name"] == "push" or itm["name"] == "pop":
                # if the itm name is "push" or "pop", inject different push methods because of T
                data_types = ["int", "float", "string", "bool"]

                full = f"bool {itm['name']}({data_types[0]} value);\n"
                for dt in data_types[1:]:
                    full += f"\tbool {itm['name']}({dt} value);\n"
        if dump == "jjPIXELMAPList":
            if itm["name"] == "height":
                # uint8& operator [] (uint x, uint y)
                # const uint8& operator [] (uint x, uint y) const
                origFull = full
                full = f"uint8& opIndex(uint x, uint y);\n\tconst uint8& opIndex(uint x, uint y);\n\t"
                full += "jjPIXELMAP();\n\tjjPIXELMAP(uint width, uint height);\n\tjjPIXELMAP();\n\tjjPIXELMAP(const jjANIMFRAME@ animFrame);"
                full += f"\n\t{origFull}"
        if itm["type"] == "property":
            if itm["name"] in ddump:
                continue
            ddump.append(itm["name"])
        output += f"    {full}\n"
    
    # more stupid hacks
    if dump == "jjPALCOLORList":
        output += f"\tjjPALCOLOR(int r, int g, int b);\n"
    elif dump == "jjSTREAMList":
        output += f"\tjjSTREAM();\n\tjjSTREAM(const string &in filename);\n"

    output += f"}};\n"

defined = []
for dump in api.keys():
    if dump == "eventsList" or dump == "globalpropertiesList" or dump == "globalfunctionsList":
        for itm in api[dump]:
            full = process_full(itm["full"], itm["attributes"])
            if itm["type"] == "property":
                if itm["name"] in defined:
                    continue
                defined.append(itm["name"])
            if itm["name"] == "jjObjects":
                # hack to manuever "jjObjectMax is not a type"
                full = f"array<jjOBJ@> jjObjects = array<jjOBJ@>(4096);"
            output += f"/* {better_description(itm['description'], 0)}*/\n{full}\n"

with open("predefined/jj2.predefined", "w") as f:
    f.write(output)
    f.close()

print(f"Wrote {len(output.splitlines())} lines to jj2.predefined")
