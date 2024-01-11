import argparse

preprocessors = ["#define", "#ifdef", "#endif", "#undef", "#ifndef", "#macro", "#defmacro", "#enddef", "#else"]
optimize_newlines = True

def process(source_code: str):
    global optimize_newlines
    definitions = {}
    macros = {}
    lines = source_code.split("\n")
    output = ""
    
    ifdef_inside = False
    ifdef_allowed = False
    ifdef_line = -2 # This is for warns

    macro_inside = False
    macro_current = {}
    macro_line = -2 # This is for warns

    for line_index, line in enumerate(lines):
        unstripped_line = line
        line = line.strip()
        split = line.split(" ")

        # conditionals
        if split[0] == "#define":
            assert len(split) >= 2, "Not enough arguments!"
            definitions[split[1]] = True if len(split) == 2 else split[2]
            continue
        if split[0] == "#ifdef":
            assert len(split) >= 2, "Not enough arguments!"
            ifdef_inside = True
            ifdef_allowed = definitions.get(split[1])
            ifdef_line = line_index
            continue
        if split[0] == "#endif":
            assert ifdef_inside, "No current if condition and a #endif was present!"
            ifdef_inside = False
            ifdef_line = -2
            continue
        if split[0] == "#undef":
            assert len(split) >= 2, "Not enough arguments!"
            if definitions.get(split[1]):
                del definitions[split[1]]
            else:
                del macros[split[1]]
            continue
        if split[0] == "#ifndef":
            assert len(split) >= 2, "Not enough arguments!"
            ifdef_inside = True
            ifdef_allowed = not definitions.get(split[1])
            continue
        if split[0] == "#else":
            assert ifdef_line != -1, "You are not inside a conditional preprocessor!"
            assert ifdef_inside, "You are not inside a conditional preprocessor!"
            ifdef_allowed = not definitions.get(lines[ifdef_line].strip().split(" ")[1])
            continue
        
        if ifdef_inside and not ifdef_allowed:
            continue

        # macros
        if split[0] == "#macro":
            assert len(split) >= 2, "Not enough arguments!"
            assert macros.get(split[1]) != None, "Macro doesn't exist!"
            assert len(split[2:]) == len(macros[split[1]]["args"]), "Incorrect number of arguments for macro!"

            macro_code: str = macros[split[1]]["code"]
            for i, arg in enumerate(macros[split[1]]["args"]):
                macro_code = macro_code.replace(f"%{arg}%", split[2:][i])
            output += macro_code
            continue
        if split[0] == "#defmacro":
            assert len(split) >= 2, "Not enough arguments!"
            macro_inside = True
            macro_current = {"name": split[1], "code": "", "args": split[2:]}
            macro_line = line_index
            continue
        if split[0] == "#enddef":
            assert macro_current != {}, "A macro is not being defined currently!"

            if macro_inside:
                macros[macro_current["name"]] = macro_current
                macro_current = {}
                macro_inside = False
                macro_line = -2
            continue
        if macro_inside:
            macro_current["code"] += unstripped_line + "\n"
            continue

        # nicer looking output
        if optimize_newlines and (line == "" and lines[line_index - 1] != None and lines[line_index - 1].split(" ")[0] in preprocessors):
            continue

        output += unstripped_line + "\n"
    
    if macro_inside or ifdef_inside:
        print(f"Never ending ifdef/macro found! ifdef line: {ifdef_line + 1}, macro line: {macro_line + 1}")

    # remove the last new line
    output = output[:-1]

    for defname in definitions:
        defvalue = definitions[defname]
        output = output.replace(f"${defname}$", str(defvalue))

    return {
        "output": output,
        "definitions": definitions,
        "macros": macros
    }

parser = argparse.ArgumentParser(prog="AngelScript++", description="Enhances AngelScript with extra features", epilog="Made by Spaz Electro")
parser.add_argument("input")
parser.add_argument("output")
parser.add_argument("-On", "--optimize-newlines", default="true")

args = parser.parse_args()
optimize_newlines = True if args.optimize_newlines in ("true", "True") else False

source_code = open(args.input).read()
processed_code = process(source_code)
print(f"Success with {len(processed_code['definitions'])} definition(s) and {len(processed_code['macros'])} macros and {len(processed_code['output'].splitlines())} line(s)!")
open(args.output, "w").write(processed_code["output"])
