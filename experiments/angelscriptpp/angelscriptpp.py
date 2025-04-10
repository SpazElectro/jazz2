from datetime import datetime
from asteval import Interpreter
import argparse

ANGELSCRIPTPP_VERSION = 1_1_0
# ```cpp
# #define awesome void onMain() { jjConsole("OK"); }
# #{awesome} // turns into `void onMain() { jjConsole("OK"); }`
# ##{awesome} // turns into `"void onMain() { jjConsole("OK"); }"` (quotes)
# #asset "theasset.png" // turns into "projectname_theasset.png"
# ```
preprocessors = [
    "#define", "#undef",                           # definitions
    "#ifdef", "#endif", "#ifndef", "#if", "#else", # conditionals
    "#macro", "#defmacro", "#enddef",              # macros
    "#pragma",                                     # regions
    "#asset",                                    # util
    "#error", "#warn", "#info"                     # logging
]

optimize_newlines = True

def process(source_code: str, project_name: str | None = None, file_name: str | None = None):
    global optimize_newlines
    aeval = Interpreter()
    now = datetime.now()
    definitions = {
        "__FILE__": file_name,
        "__DATE__": now.strftime("%b %d %Y"),
        "__TIME__": now.strftime("%H:%M:%S"),
        "__VERSION__": ANGELSCRIPTPP_VERSION
    }
    default_definitions_count = len(definitions)
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

        if preprocessor == "#include":
            if line.endswith(".predefined\""):
                if not optimize_newlines: output += "//\n"
                continue
            # #include "../../scripts/STVutil.asc"
            if line.endswith(".asc\"") and "/" in line:
                output += f"#include \"{line.split('/')[-1].split('\"')[0]}\"\n"
                continue
        # conditionals
        if preprocessor == "#pragma":
            if split[1] == "region" or split[1] == "endregion":
                if not optimize_newlines: output += "//\n"
                continue
        if preprocessor == "#define":
            assert len(split) >= 2, "Not enough arguments!"
            definitions[split[1]] = "1" if len(split) == 2 else " ".join(split[2:])
            if not optimize_newlines: output += "//\n"
            continue
        if preprocessor == "#ifdef":
            assert len(split) >= 2, "Not enough arguments!"
            ifdef_inside = True
            ifdef_allowed = definitions.get(split[1])
            ifdef_line = line_index
            if not optimize_newlines: output += "//\n"
            continue
        if preprocessor == "#endif":
            assert ifdef_inside, "No current if condition and a #endif was present!"
            ifdef_inside = False
            ifdef_line = -2
            if not optimize_newlines: output += "//\n"
            continue
        if preprocessor == "#undef":
            assert len(split) >= 2, "Not enough arguments!"
            if definitions.get(split[1]):
                del definitions[split[1]]
            else:
                del macros[split[1]]
            if not optimize_newlines: output += "//\n"
            continue
        if preprocessor == "#ifndef":
            assert len(split) >= 2, "Not enough arguments!"
            ifdef_inside = True
            ifdef_allowed = not definitions.get(split[1])
            if not optimize_newlines: output += "//\n"
            continue
        if preprocessor == "#else":
            assert ifdef_line != -1, "You are not inside a conditional preprocessor!"
            assert ifdef_inside, "You are not inside a conditional preprocessor!"
            ifdef_allowed = not ifdef_allowed
            if not optimize_newlines: output += "//\n"
            continue
        if preprocessor == "#if":
            assert len(split) >= 2, "Not enough arguments!"
            aeval.symtable = definitions
            aeval.symtable["__LINE__"] = line_index + 1

            ifdef_allowed = aeval(" ".join(split[1:]))
            if len(aeval.error) > 0:
                print(f"^^^ Line {line_index+1} of {file_name} ^^^")
                break
            ifdef_inside = True
            ifdef_line = line_index
            if not optimize_newlines: output += "//\n"
            continue
        
        if ifdef_inside and not ifdef_allowed:
            if not optimize_newlines: output += "//\n"
            continue

        # macros
        if preprocessor == "#macro":
            assert len(split) >= 2, "Not enough arguments!"
            assert macros.get(split[1]) != None, "Macro doesn't exist!"
            # assert len(split[2:]) == len(macros[split[1]]["args"]), "Incorrect number of arguments for macro!"

            symbol = "$"
            macro_code: str = macros[split[1]]["code"]
            for i, arg in enumerate(macros[split[1]]["args"]):
                macro_code = macro_code.replace(f"{symbol}{arg}{symbol}", split[2:][i])
            output += macro_code
            if not optimize_newlines: output += "//\n"
            continue
        if preprocessor == "#defmacro":
            assert len(split) >= 2, "Not enough arguments!"
            macro_inside = True
            macro_current = {"name": split[1], "code": "", "args": split[2:]}
            macro_line = line_index
            if not optimize_newlines: output += "//\n"
            continue
        if preprocessor == "#enddef":
            assert macro_current != {}, "A macro is not being defined currently!"

            if macro_inside:
                macros[macro_current["name"]] = macro_current
                macro_current = {}
                macro_inside = False
                macro_line = -2
            if not optimize_newlines: output += "//\n"
            continue
        if macro_inside:
            macro_current["code"] += unstripped_line + "\n"
            if not optimize_newlines: output += "//\n"
            continue
        
        # #asset "some.png"
        if preprocessor.startswith("#asset"):
            assert project_name != None, "Project name is not set!"
            assert len(split) >= 2, "Not enough arguments!"
            assert line.startswith("#asset \""), "Missing quotes! (must be double quotes)"
            assert line.split("//")[0].strip().endswith("\""), "Unescaped quotes!"
            
            psplit = line.split("\"")
            filename = psplit[1]
            output += f"\"{project_name}_{filename}\"\n"
            
            continue
		# #{__LINE__} for plain text, ${__LINE__} for strings
        if preprocessor.startswith("#{") or preprocessor.startswith("##{"):
            assert "}" in preprocessor, "Not enough arguments!"
            # start_delim = preprocessor[0]
            start_delim = preprocessor[1]
            s = preprocessor.split("{")[1].split("}")[0]
            if s == "__LINE__":
                defn = str(line_index + 1)
            else:
                defn = definitions.get(s)
            assert defn is not None, f"Definition was not found! definition: {s}"
            remaining = "}".join(line.split("}")[1:])
            if start_delim == "{":
                output += f"{defn}{remaining}\n"
            elif start_delim == "#":
                output += f"\"{defn}\"{remaining}\n"
            continue
        # "#error", "#warn", "#info"
        if preprocessor == "#error" or preprocessor == "#warn" or preprocessor == "#info":
            assert len(split) >= 2, "Not enough arguments!"
            message = " ".join(split[1:])
            if message.startswith("\"") and message.endswith("\""):
                message = message[1:-1]
            color = "\033[91m" if preprocessor == "#error" else "\033[93m" if preprocessor == "#warn" else "\033[94m"
            reset = "\033[0m"
            print(f"{color}{preprocessor[1:]}: {message} in {file_name}:{line_index + 1}{reset}")
            continue

        # nicer looking output
        if optimize_newlines and (line == "" and lines[line_index - 1] != None and lines[line_index - 1].split(" ")[0] in preprocessors):
            if not optimize_newlines: output += "//\n"
            continue

        output += unstripped_line + "\n"
    
    if macro_inside or ifdef_inside:
        print(f"Never ending ifdef/macro found! ifdef line: {ifdef_line + 1}, macro line: {macro_line + 1}")

    # remove the last new line
    output = output[:-1]

    # Keeping this might cause accidental usage and debugging issues, so preprocessors are preferred.
    # for defname in definitions:
    #     defvalue = definitions[defname]
    #     output = output.replace(f"${defname}$", str(defvalue))

    return {
        "output": output,
        "definitions": definitions,
        "definition_count": len(definitions) - default_definitions_count,
        "macros": macros
    }

parser = argparse.ArgumentParser(prog="AngelScript++", description="Enhances AngelScript with extra features", epilog="Made by Spaz Electro")
parser.add_argument("input")
parser.add_argument("output")
parser.add_argument("-P", "--project-name", default=None)
parser.add_argument("-On", "--optimize-newlines", default="false")

args = parser.parse_args()
optimize_newlines = True if args.optimize_newlines in ("true", "True") else False
project_name = args.project_name

source_code = open(args.input).read()
processed_code = process(source_code, project_name, args.input)
print(f"Success with {processed_code['definition_count']} definition(s) and {len(processed_code['macros'])} macros and {len(processed_code['output'].splitlines())} line(s)!")
open(args.output, "w").write(processed_code["output"])
