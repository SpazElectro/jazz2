import json

jsonA = json.load(open("classes.json"))
jsonB = json.load(open("global.json"))
output = {}

for x in jsonA:
    output[x] = jsonA[x]
for y in jsonB:
    output[y] = jsonB[y]

with open("combined.json", "w") as f:
    json.dump(output, f)
