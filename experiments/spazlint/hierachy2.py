import json

reserved = ["const", "&in", "&out", "&inout", "private"]

def findFunction(lines, cursorLine):
    className = ""
    fnLineIndex = -1
    args = []
    fnName = ""
    returns = ""
    fullLine = ""

    for lineIndex, lineX in enumerate(lines):
        line = lineX.strip()
        split = line.split(" ")

        if split[0] == "class":
            className = split[1]
        elif lineX.split(" ")[0] == "}" and className != "":
            className = ""

        if (len(split) >= 3 and "(" in split[2]) or (len(split) >= 2 and "(" in split[1]) or (len(split) >= 1 and "(" in split[0]): 
            if split[0] == "funcdef":
                continue
            # this wont work if the return type starts with any of these keywords
            if line.startswith(("if", "for", "switch", "while")):
                continue
            if len(split) >= 2 and "function" in split[1].split("(")[0]:
                continue

            if split[-1] == "{" or lines[lineIndex + 1].strip() == "{":
                constructor = ("(" in split[0])
                fnName = split[0 if constructor else 1].split("(")[0]
                
                # is this even a real keyword in angelscript???
                if split[0] in ["private", "public"]:
                    fnName = split[1 if constructor else 2].split("(")[0]
                
                fnLineIndex = lineIndex + 1
                returns = line.split(" ")[0]
                retIndex = 0
                retIterations = 0
                fullLine = line

                if constructor:
                    returns = fnName
                else:
                    while True:
                        if retIterations >= 10:
                            print(f"[WARN] Over 10 ret interactions have passed! fnName: {fnName}, line: {line}")
                            break
                        if len(line.split(" ")) > retIndex+1 and line.split(" ")[retIndex+1].split("(")[0] == fnName:
                            break
                        
                        returns += line.split(" ")[retIndex]
                        retIndex += 1
                        retIterations += 1
                    
                    if "@" in fnName:
                        fnName = removeHandle(fnName)
                        returns = returns+"@"

                args = ' '.join(split[(0 if constructor else 1):]).split("(")[1:]
                if ' '.join(args).strip().startswith(")"):
                    args = []
                else:
                    args = ' '.join(args)
                    args = args.split(")")[0]
                    args = ''.join(args)
                    realArgs = []
                    attributes = []
                    for x in args.split(", "):
                        x = x.strip()
                        if x == "function":
                            continue
                        argType = x.split(" ")[0]
                        argName = x.split(" ")[1]
                        argTypeIndex = 0
                        argNameIndex = 1
                        if "@" in argName:
                            argName = removeHandle(argName)
                            argType = argType + "@"
                        
                        while argType in reserved:
                            argType = x.split(" ")[argTypeIndex]
                            if argType in reserved:
                                attributes.append(argType.strip())
                            argTypeIndex += 1
                        while argName in reserved:
                            argName = x.split(" ")[argNameIndex]
                            if argName in reserved:
                                attributes.append(argName.strip())
                            argNameIndex += 1
                        
                        realArgs.append({
                            "type": argType.strip(),
                            "name": argName.strip(),
                            "attributes": attributes
                        })
                    
                    args = realArgs
                
        if lineIndex > cursorLine:
            break

    if fnLineIndex != -1:
        # {
        #     "type": prop["type"],
        #     "name": prop["name"],
        #     "description": prop["description"],
        #     "full": prop["full"],
        #     "items": prop["arguments"]
        # }
        # -2 again, why?
        description = lines[fnLineIndex - 2].strip()
        if description != "" and description.startswith("//"):
            description = '//'.join(description.split("//")[1:]).strip()
        else:
            description = ""

        return {
            "type": "function",
            "name": fnName,
            "description": description,
            "full": fullLine,
            "arguments": args,
            "line": fnLineIndex,
            # "class": className,
            # "returns": returns,
            # "err": "none"
        }
    else:
        return {
            "err": "not-found"
        }

def removeHandle(clType: str):
    return clType.replace("@", "")

def removeHandlesFromArgs(args):
    newArgs = []

    for x in args:
        newX = x
        x["type"] = removeHandle(x["type"])
        newArgs.append(newX)
    
    return newArgs

def getDatatypes(lines):
    datatypes = []
    
    for lineIndex, lineX in enumerate(lines):
        line = lineX.strip()
        split = line.split(" ")

        if split[0] == "class":
            datatypes.append(removeHandle(split[1]))
        elif split[0] == "funcdef":
            datatypes.append(removeHandle(split[2].split("(")[0]))
    
    datatypes.extend([
        "void", "int8", "int16", "int", "int64", "uint8", "uint16", "uint", "uint64", "float", "double", "bool", "string",
        "jjPLAYER", "jjOBJ", "jjPARTICLE", "jjCONTROLPOINT", "jjCHARACTER", "jjWEAPON", "jjSTREAM", "jjBEHAVIOR", "jjRNG", "jjPAL", "jjPALCOLOR", "jjCANVAS", "jjLAYER", "jjTEXTAPPEARANCE", "jjPLAYERDRAW", "jjANIMSET", "jjANIMATION", "jjANIMFRAME", "jjTILE", "jjPIXELMAP", "jjMASKMAP",
        "dictionary"
    ])

    return tuple(datatypes)

def getDataTypeOf(x):
    x = x.split(" ")

    if x[0].split(" ")[0] in reserved:
        x = ' '.join(x).split(" ")[1:]
    
    if ' '.join(x).startswith("array<"):
        # TODO: check if this is even needed
        return "array<" + removeHandle(' '.join(x)[6:].split(">")[0]) + ">"

    return removeHandle(x[0])

def getNameOf(dt, line):
    split = line.split(" ")
    name = ""

    for xIndex, x in enumerate(split):
        if not x in reserved and xIndex != 0 and x != ">" and not x in dt:
            if x.endswith(";"):
                x = x[:-1]
            elif len(x.split("(")) > 1:
                x = x.split("(")[0]
            name = x
            break
    
    return name

def isDataType(dt, x):
    t = getDataTypeOf(' '.join(x))

    if t.startswith("array<"):
        t = t[6:].split(">")[0]

    return t.strip() in dt

def getGlobalScopeVariables(lines):
    dataTypes = getDatatypes(lines)
    output = []

    for lineIndex, lineX in enumerate(lines):
        line: str = lineX.strip()

        if len(line.split(" ")) >= 2:
            if isDataType(dataTypes, line.split(" ")[:-1]):
                if lineX.startswith("\t") or lineX.startswith(" "):
                    continue
                if lineX.endswith(") {") or lineX.endswith("){"):
                    continue
                if len(lines) > lineIndex+1 and lines[lineIndex + 1].strip() == "{":
                    continue
                
                output.append({
                    "type": getDataTypeOf(removeHandle(line)),
                    "name": getNameOf(dataTypes, removeHandle(line)),
                    "line": lineIndex
                })

    return output

def getGlobalScopeFunctions(lines):
    output = []

    for lineIndex, lineX in enumerate(lines):
        line: str = lineX.strip()
        if line.startswith("//"):
            continue
        split = line.split(" ")
        if len(split) >= 2:
            if line.endswith("{") or line.endswith(")"):
                if lineX.startswith("\t") or lineX.startswith(" "):
                    continue
                if line.startswith("if(") or line.startswith("if ("):
                    continue
                if line.startswith("switch(") or line.startswith("switch ("):
                    continue
                if line.startswith("while(") or line.startswith("while ("):
                    continue
                if line.startswith("for(") or line.startswith("for ("):
                    continue
                if "= {" in line or "={" in line:
                    continue
                if split[0] == "enum" or split[0] == "class":
                    continue

                output.append(findFunction(lines, lineIndex))

    return output


def getLocalScopeVariables(lines, cursorLine):
    fn = findFunction(lines, cursorLine)
    if fn.get("err"):
        return []
    dataTypes = getDatatypes(lines)
    output = []
    
    for lineIndex, lineX in enumerate(lines):
        line: str = lineX.strip()

        if lineIndex < fn["line"]:
            continue
        if lineIndex > cursorLine:
            break

        if len(line.split(" ")) >= 2:
            if isDataType(dataTypes, line.split(" ")[:-1]):
                if lineX.endswith(") {") or lineX.endswith("){"):
                    continue
                if lines[lineIndex + 1].strip() == "{":
                    continue
                
                output.append({
                    "type": getDataTypeOf(removeHandle(line)),
                    "name": getNameOf(dataTypes, removeHandle(line)),
                    "line": lineIndex
                })

    return output 

if __name__ == "__main__":
    from pprint import pprint
    script = open("../../scripts/STVutil.asc").read()
    lines = script.splitlines()
    # print(getLocalScopeVariables(lines, 736))
    pprint(
    getGlobalScopeFunctions(lines)
    )

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

