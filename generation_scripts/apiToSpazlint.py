import json

api = json.load(open("api.json"))
apiFunctions = json.load(open("spazlint.json"))
output = []

for x in api:
    output.append({
        "type": "property", "properties": x
    })
for x in apiFunctions:
    output.append({
        "type": "function", "properties": x
    })

with open("outputspazlint.json", "w") as f:
    f.write(json.dumps(output))