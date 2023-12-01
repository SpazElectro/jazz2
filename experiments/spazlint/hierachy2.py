# TODO insideComment(lines, cursorLine)

from typing import List, TypedDict, Tuple, Union
import json

class ASArg(TypedDict):
    type: str
    name: str
    attributes: List[str]

class ASFunc(TypedDict):
    type: str
    name: str
    description: str
    full: str
    arguments: List[ASArg]
    line: int
    err: Union[str, None]

reserved = ["const", "&in", "&out", "&inout", "private"]

def remove_reserved_keywords(full: str, start_index=0) -> Tuple[str, List[str]]:
    attributes = []
    split_line = full.split(" ")
    tp = split_line[start_index]

    while tp in reserved:
        attributes.append(tp.strip())
        start_index += 1
        tp = split_line[start_index]

    return tp, attributes

def find_function(lines: List[str], cursorLine: int) -> ASFunc:
    className = ""
    fnLineIndex = -1
    args = []
    fnName = ""
    returns = ""
    fullLine = ""

    for lineIndex, line in enumerate(lines):
        line = line.strip()
        split = line.split(" ")

        if split[0] == "class":
            className = split[1]
        elif split[0] == "}" and className != "":
            className = ""

        if any(keyword in split for keyword in ["funcdef", "if", "for", "switch", "while", "function"]):
            continue

        if any("(" in part for part in split[:3]):
            if split[-1] == "{" or lines[lineIndex + 1].strip() == "{":
                constructor = "(" in split[0]
                fnName = split[0 if constructor else 1].split("(")[0]

                if split[0] in ["private", "public"]:
                    fnName = split[1 if constructor else 2].split("(")[0]

                fnLineIndex = lineIndex + 1
                returns = line.split(" ")[0]

                if constructor:
                    returns = fnName
                else:
                    retIndex = 0
                    retIterations = 0
                    while retIterations < 10 and len(line.split(" ")) > retIndex + 1 and line.split(" ")[retIndex + 1].split("(")[0] != fnName:
                        returns += line.split(" ")[retIndex]
                        retIndex += 1
                        retIterations += 1

                    if "@" in fnName:
                        fnName = remove_handle(fnName)
                        returns += "@"

                args = ' '.join(split[(0 if constructor else 1):]).split("(")[1:]

                if args and args[0].strip().startswith(")"):
                    args = []
                else:
                    args = [arg.strip() for arg in args[0].split(", ")]
                    realArgs = []

                    for x in args:
                        if x == "function":
                            continue

                        argType, attrsA = remove_reserved_keywords(x)
                        argName, attrsB = remove_reserved_keywords(x, 1)

                        if "@" in argName:
                            argName = remove_handle(argName)
                            argType += "@"
                        if argName.endswith(")"):
                            argName = argName[:-1]

                        realArgs.append({
                            "type": argType,
                            "name": argName,
                            "attributes": attrsA + attrsB
                        })

                    args = realArgs

        if lineIndex > cursorLine:
            break

    if fnLineIndex != -1:
        description_line = lines[fnLineIndex - 2].strip()
        description = ' '.join(description_line.split("//")[1:]).strip() if description_line.startswith("//") else ""
        
        return {
            "type": "function",
            "name": fnName,
            "description": description,
            "full": fullLine,
            "arguments": args,
            "line": fnLineIndex,
            "err": None
        }
    else:
        return {
            "type": "",
            "name": "",
            "description": "",
            "full": "",
            "arguments": [],
            "line": -1,
            "err": "not-found"
        }

def remove_handle(clType: str) -> str:
    return clType.replace("@", "")

def remove_handles_from_args(args: List[ASArg]) -> List[ASArg]:
    newArgs = []

    for x in args:
        newX = x
        x["type"] = remove_handle(x["type"])
        newArgs.append(newX)
    
    return newArgs

def get_datatypes(lines: List[str]) -> Tuple[str, ...]:
    datatypes = []
    
    for lineIndex, lineX in enumerate(lines):
        line = lineX.strip()
        split = line.split(" ")

        if split[0] == "class":
            datatypes.append(remove_handle(split[1]))
        elif split[0] == "funcdef":
            datatypes.append(remove_handle(split[2].split("(")[0]))
    
    datatypes.extend([
        "void", "int8", "int16", "int", "int64", "uint8", "uint16", "uint", "uint64", "float", "double", "bool", "string",
        "jjPLAYER", "jjOBJ", "jjPARTICLE", "jjCONTROLPOINT", "jjCHARACTER", "jjWEAPON", "jjSTREAM", "jjBEHAVIOR", "jjRNG", "jjPAL", "jjPALCOLOR", "jjCANVAS", "jjLAYER", "jjTEXTAPPEARANCE", "jjPLAYERDRAW", "jjANIMSET", "jjANIMATION", "jjANIMFRAME", "jjTILE", "jjPIXELMAP", "jjMASKMAP",
        "dictionary"
    ])

    return tuple(datatypes)

def get_datatype_of(x: str) -> str:
    splitX: List[str] = x.split(" ")

    if splitX[0].split(" ")[0] in reserved:
        splitX = ' '.join(splitX).split(" ")[1:]
    
    if ' '.join(splitX).startswith("array<"):
        # TODO: check if this is even needed
        return "array<" + remove_handle(' '.join(splitX)[6:].split(">")[0]) + ">"

    return remove_handle(splitX[0])

def get_name_of(dts: List[str], line: str) -> str:
    split = line.split(" ")
    name = ""

    for xIndex, x in enumerate(split):
        if not x in reserved and xIndex != 0 and x != ">" and not x in dts:
            if x.endswith(";"):
                x = x[:-1]
            elif len(x.split("(")) > 1:
                x = x.split("(")[0]
            name = x
            break
    
    return name

def is_datatype(dt, x) -> bool:
    t = get_datatype_of(' '.join(x))

    if t.startswith("array<"):
        t = t[6:].split(">")[0]

    return t.strip() in dt
def get_global_scope_variables(lines: List[str], insideNamespace=False, startNamespaceLine=0):
    dataTypes = get_datatypes(lines)
    output = []

    for lineIndex, lineX in enumerate(lines):
        line: str = lineX.strip()

        split_line = line.split(" ")
        if len(split_line) < 2:
            continue

        if is_datatype(dataTypes, split_line[:-1]):
            fn = find_function(lines, lineIndex)
            if fn.get("err") == "not-found":
                if not insideNamespace and (lineX.startswith("\t") or lineX.startswith(" ")):
                    continue
                if line.endswith(") {") or line.endswith("){") or (len(lines) > lineIndex + 1 and lines[lineIndex + 1].strip() == "{"):
                    continue

                output.append({
                    "type": get_datatype_of(remove_handle(line)),
                    "name": get_name_of(list(dataTypes), remove_handle(line)),
                    "line": startNamespaceLine + lineIndex
                })

    return output

def get_global_scope_functions(lines, insideNamespace=False):
    output = []

    for lineIndex, lineX in enumerate(lines):
        line: str = lineX.strip()

        if line.startswith("//") or line.startswith("enum") or line.startswith("class") or line.startswith("namespace"):
            continue

        split_line = line.split(" ")
        if len(split_line) < 2:
            continue

        if line.endswith("{") or line.endswith(")"):
            if not insideNamespace and (lineX.startswith("\t") or lineX.startswith(" ")):
                continue

            if line.startswith("if(") or line.startswith("if (") or line.startswith("switch(") or line.startswith("switch (") or line.startswith("while(") or line.startswith("while (") or line.startswith("for(") or line.startswith("for ("):
                continue

            if "= {" in line or "={" in line:
                continue

            output.append(find_function(lines, lineIndex))

    return output

def get_namespaces(lines):
    output = []
    nsN = ""
    nsL = 0

    for lineIndex, lineX in enumerate(lines):
        line: str = lineX.strip()
        if line.startswith("//"):
            continue
        split = line.split(" ")
        if len(split) >= 2:
            if split[0] != "namespace":
                continue
            nsN = split[1]
            nsL = lineIndex
        
        if nsN != "" and (line == "}" or line == "};"):
            output.append({
                "name": nsN, "line": nsL, "endLine": lineIndex,
                "vars": get_global_scope_variables(lines[nsL:lineIndex], True, nsL),
                "funs": get_global_scope_functions(lines[nsL:lineIndex], True)
            })
            nsN = ""

    return output

def get_local_scope_variables(lines, cursorLine):
    fn = find_function(lines, cursorLine)
    if fn.get("err") == "not-found":
        return []
    dataTypes = get_datatypes(lines)
    output = []
    
    for lineIndex, lineX in enumerate(lines):
        line: str = lineX.strip()

        if lineIndex < fn["line"]:
            continue
        if lineIndex > cursorLine:
            break

        if len(line.split(" ")) >= 2:
            if is_datatype(dataTypes, line.split(" ")[:-1]):
                if lineX.endswith(") {") or lineX.endswith("){"):
                    continue
                if lines[lineIndex + 1].strip() == "{":
                    continue
                
                output.append({
                    "type": get_datatype_of(remove_handle(line)),
                    "name": get_name_of(list(dataTypes), remove_handle(line)),
                    "line": lineIndex
                })

    return output 

if __name__ == "__main__":
    from pprint import pprint
    script = open("../../scripts/STVutil.asc").read()
    lines = script.splitlines()
    # print(getLocalScopeVariables(lines, 736))
    # pprint(
    # getNamespaces(lines)[0]
    # )
    suggestions = []
    def handleProp(prop):
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
    line = "Test::"
    for namespace in get_namespaces(lines):
        if line.split("::")[0] == namespace["name"]:
            for p in namespace["vars"]:
                handleProp({**p, **{"description": ""}})
            for p in namespace["funs"]:
                handleProp({**p, **{"description": ""}})
    print(suggestions)

    # print(getGlobalScopeVariables(lines))
    # fnc = findFunction(lines, 711)
    # line = "player."
    
    # if fnc["err"] == "not-found":
    #     print("function not found")
    # else:
    #     print("function found")
    #     fnc["args"] = removeHandlesFromArgs(fnc["args"])
    #     for x in fnc["args"]:
    #         if x["name"] == line.split(".")[0]:
    #             print(f"found {x['name']}")

