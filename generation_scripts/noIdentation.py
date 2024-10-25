import json

with open("outputspazlint.json", "w") as f:
    f.write(json.dumps(json.load(open("playerapi.json")))) # identation