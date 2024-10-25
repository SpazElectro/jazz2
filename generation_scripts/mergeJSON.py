import json

jsonA = json.load(open("globalfuncs.json"))
jsonB = json.load(open("globalprops.json"))
output = []

for x in jsonA:
    output.append(x)
for y in jsonB:
    output.append(y)

with open("combined.json", "w") as f:
    json.dump(output, f)