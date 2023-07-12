import json, sys
import errorchecker

properties: list = json.load(open("G:\steve\jazz2stuff\jazz2\experiments\spazlint\spazlint.json"))

# the most important thing is *syntax*, what I mean is instead of going like:
# ```
# jjConsole(
#   "hello world"
# );
# ```
# you just do `jjConsole("hello world");`

class JJ2PlusLinter:
    def __init__(self, file):
        self.file = file
        self.code = open(file).read()
        self.enabled_errors = ["semicolons"]
        # self.enabled_errors = []

        disabledErrors = self.code.split("\n")[0]
        if disabledErrors.startswith("// !ignore-"):
            err = disabledErrors.split("// !ignore-")[1]

            if err in self.enabled_errors:
                self.enabled_errors.remove(err)

    def lint(self, advanced=False):
        linting_errors = []

        # obviously this isnt gonna work that great but hey it gets the job done I guess
        # Rule: Check for missing semicolons at the end of lines
        lines = self.code.split("\n")

        if "semicolons" in self.enabled_errors:
            for line_index, line in enumerate(lines):
                line = line.strip()

                if line and not line.endswith(";"):
                    if line == "" or line.endswith("{") or line.endswith("}") or \
                        line.startswith("#") or line.startswith("//") or \
                        line.startswith("if") or "//" in line or \
                        line.endswith(",") or line.endswith("(") or \
                        line.startswith("else") or line.endswith("else") or \
                        line.startswith("for") or (line.startswith("case") and line.endswith(":")) or \
                        line.startswith("default:") or line.endswith("&&"):
                        continue
                    if line_index != 0 and lines[line_index - 1].strip() == "// @ignore-semicolons":
                        continue
                    if lines[line_index + 1].strip() == "{" or lines[line_index + 1].strip() == ");" or lines[line_index + 1].strip() == "};":
                        continue
                    if line_index != 0 and lines[line_index - 1].strip().startswith("for"):
                        continue

                    linting_errors.append({
                        "line": line_index,
                        "char": 0,
                        "text": f"Missing semicolon.",
                        "type": "ERR"
                    })
        
        if advanced:
            for e in errorchecker.getErrors(self.file):
                linting_errors.append({
                    "line": int(e.split("(")[1].split(", ")[0].split(") : ")[0]) - 1,
                    "char": int(e.split("(")[1].split(", ")[1].split(") : ")[0]) - 1,
                    "text": e.split("(")[1].split(") : ")[1].split(" : ")[1],
                    "type": e.split("(")[1].split(") : ")[1].split(" : ")[0]
                })
        
        return linting_errors

    def autocomplete(self, line):
        line = line.strip().lower()
        suggestions = []

        for prop in properties:
            name = prop["properties"]["name"]

            # optimization
            if name.lower().startswith(line):
                if prop["type"] == "function":
                    suggestions.append({
                        "type": prop["type"],
                        "name": name,
                        "description": prop["properties"]["description"],
                        "arguments": prop["properties"]["full"].split(" ")[1:],
                        "returns": prop["properties"]["full"].split(" ")[0],
                        "items": prop["properties"]["arguments"]
                    })
                else:
                    suggestions.append({
                        "type": prop["type"],
                        "name": name,
                        "description": prop["properties"]["description"]
                    })
        
        return suggestions

file = sys.argv[1]
line = sys.argv[2]
char = int(sys.argv[3])
advanced = True if sys.argv[4] == "true" else False

linter = JJ2PlusLinter(file)
x = linter.autocomplete(line[char:])

if len(x) == 0:
    x = linter.autocomplete(line[:char])

print(json.dumps(x))
print(json.dumps(linter.lint(advanced)))
