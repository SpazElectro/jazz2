reserved = ["const", "&in", "&out", "&inout", "private"]

def findFunction(lines, cursorLine):
    className = ""
    fnLineIndex = -1
    args = []

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
            if line.strip().startswith(("if", "for", "switch", "while")):
                continue
            if len(split) >= 2 and "function" in split[1].split("(")[0]:
                continue

            if split[-1] == "{" or lines[lineIndex + 1].strip() == "{":
                constructor = ("(" in split[0])
                fnName = split[0 if constructor else 1].split("(")[0]
                
                if split[0] in ["private", "public"]:
                    fnName = split[1 if constructor else 2].split("(")[0]
                
                fnLineIndex = lineIndex + 1

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
                        if argName.startswith("@"):
                            argType = argType + "@"
                            argName = argName[1:]
                        
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
                
        if lineIndex >= cursorLine:
            break

    if fnLineIndex != -1:
        return {
            "args": args,
            "name": fnName,
            "line": fnLineIndex,
            "class": className,
            "err": "none"
        }
    else:
        return {
            "err": "not-found"
        }

def removeHandle(clType: str):
    while "@" in clType:
        clType = clType.replace("@", "")
    
    return clType

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

def isDataType(dt, x):
    if x[0].split(" ")[0] in reserved:
        x = ' '.join(x).split(" ")[1:]

    if ' '.join(x).startswith("array<"):
        return removeHandle(' '.join(x)[6:].split(">")[0]) in dt
    
    return removeHandle(x[0]) in dt

def getGlobalScopeVariables(lines):
    dataTypes = getDatatypes(lines)

    for lineIndex, lineX in enumerate(lines):
        line: str = lineX.strip()

        # if lineIndex == 1401:
        #     print(line)
        #     print(isDataType(dataTypes, line.split(" ")[:-1]))
        #     break
            
        if len(line.split(" ")) >= 2:
            # print(lineIndex+1)
            if isDataType(dataTypes, line.split(" ")[:-1]):
                if lineX.startswith("\t") or lineX.startswith(" "):
                    continue
                if lineX.endswith(") {") or lineX.endswith("){"):
                    continue
                if lines[lineIndex + 1].strip() == "{":
                    continue
                print(lineX)
                # print(f"var? {line}")

if __name__ == "__main__":
    script = open("../../scripts/STVutil.asc").read()
    lines = script.splitlines()
    getGlobalScopeVariables(lines)
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

