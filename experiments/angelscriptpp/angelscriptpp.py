import argparse

preprocessors = [
    "#define", "#undef", # definitions
    "#ifdef", "#endif", "#ifndef", "#else", # conditionals
    "#macro", "#defmacro", "#enddef", # macros
    "#pragma", # regions
    "#texture" # util
]

optimize_newlines = True
is_debugging = True

def process(source_code: str, project_name: str | None = None):
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
        preprocessor = split[0]

        # conditionals
        if preprocessor == "#pragma":
            if split[1] == "region" or split[1] == "endregion":
                if is_debugging: output += "//\n"
                continue
        if preprocessor == "#define":
            assert len(split) >= 2, "Not enough arguments!"
            definitions[split[1]] = True if len(split) == 2 else split[2]
            if is_debugging: output += "//\n"
            continue
        if preprocessor == "#ifdef":
            assert len(split) >= 2, "Not enough arguments!"
            ifdef_inside = True
            ifdef_allowed = definitions.get(split[1])
            ifdef_line = line_index
            if is_debugging: output += "//\n"
            continue
        if preprocessor == "#endif":
            assert ifdef_inside, "No current if condition and a #endif was present!"
            ifdef_inside = False
            ifdef_line = -2
            if is_debugging: output += "//\n"
            continue
        if preprocessor == "#undef":
            assert len(split) >= 2, "Not enough arguments!"
            if definitions.get(split[1]):
                del definitions[split[1]]
            else:
                del macros[split[1]]
            if is_debugging: output += "//\n"
            continue
        if preprocessor == "#ifndef":
            assert len(split) >= 2, "Not enough arguments!"
            ifdef_inside = True
            ifdef_allowed = not definitions.get(split[1])
            if is_debugging: output += "//\n"
            continue
        if preprocessor == "#else":
            assert ifdef_line != -1, "You are not inside a conditional preprocessor!"
            assert ifdef_inside, "You are not inside a conditional preprocessor!"
            ifdef_allowed = not definitions.get(lines[ifdef_line].strip().split(" ")[1])
            if is_debugging: output += "//\n"
            continue
        
        if ifdef_inside and not ifdef_allowed:
            if is_debugging: output += "//\n"
            continue

        # macros
        if preprocessor == "#macro":
            assert len(split) >= 2, "Not enough arguments!"
            assert macros.get(split[1]) != None, "Macro doesn't exist!"
            # assert len(split[2:]) == len(macros[split[1]]["args"]), "Incorrect number of arguments for macro!"

            macro_code: str = macros[split[1]]["code"]
            for i, arg in enumerate(macros[split[1]]["args"]):
                macro_code = macro_code.replace(f"%{arg}%", split[2:][i])
            output += macro_code
            if is_debugging: output += "//\n"
            continue
        if preprocessor == "#defmacro":
            assert len(split) >= 2, "Not enough arguments!"
            macro_inside = True
            macro_current = {"name": split[1], "code": "", "args": split[2:]}
            macro_line = line_index
            if is_debugging: output += "//\n"
            continue
        if preprocessor == "#enddef":
            assert macro_current != {}, "A macro is not being defined currently!"

            if macro_inside:
                macros[macro_current["name"]] = macro_current
                macro_current = {}
                macro_inside = False
                macro_line = -2
            if is_debugging: output += "//\n"
            continue
        if macro_inside:
            macro_current["code"] += unstripped_line + "\n"
            if is_debugging: output += "//\n"
            continue
        
        # #texture("Cerveza_Cristal.png") turns into
        # "STVgodhelpme_Cerveza_Cristal.png"
        if preprocessor.startswith("#texture"):
            assert project_name != None, "Project name is not set!"
            assert preprocessor.startswith("#texture("), "Missing brackets!"
            assert preprocessor.startswith("#texture(\""), "Missing quotes! (must be double quotes)"
            assert preprocessor.endswith("\")"), "Unescaped quotes/brackets!"
            
            psplit = preprocessor.split("(")
            filename = psplit[0].split("\"")[1].split("\"")[0]
            output += f"\"{project_name}_{filename}\"\n"
            
            continue

        # nicer looking output
        if optimize_newlines and (line == "" and lines[line_index - 1] != None and lines[line_index - 1].split(" ")[0] in preprocessors):
            if is_debugging: output += "//\n"
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
parser.add_argument("-P", "--project-name", default=None)
parser.add_argument("-On", "--optimize-newlines", default="true")

args = parser.parse_args()
optimize_newlines = True if args.optimize_newlines in ("true", "True") else False
project_name = args.project_name

source_code = open(args.input).read()
processed_code = process(source_code, project_name)
print(f"Success with {len(processed_code['definitions'])} definition(s) and {len(processed_code['macros'])} macros and {len(processed_code['output'].splitlines())} line(s)!")
open(args.output, "w").write(processed_code["output"])
