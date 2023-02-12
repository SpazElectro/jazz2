# var it = 0
# var items = []
# var prevDescription = '';

# function findNextDD(element, startIndex) {
#   let resultx = "";
#   let counter = 0;
#   for (let i = startIndex; i < element.children.length; i++) {
#     if (element.children[i].tagName === "DD") {
#       resultx += element.children[i].textContent + " ";
#     } else if (element.children[i].tagName === "DT") {
#       if (prevDescription === resultx) {
#         continue;
#       }
#       prevDescription = resultx;
#       break;
#     }
#     counter++;
#     if (counter === 10) {
#       break;
#     }
#   }
#   prevDescription = '';
#   return resultx;
# }

# for (let i = 0; i < abc.children.length; i++) {
#   if (abc.children[i].tagName === "DT") {
#     const name = abc.children[i].getElementsByClassName("name")[0].id;
#     const description = findNextDD(abc, i + 1);
#     items.push({
#       name: name,
#       description: description
#     });
#   }
# }

import json

result = {}

for item in json.load(open("api.json")):
    result[item["name"]] = {
        "prefix": item["name"],
        "body": item["name"],
        "description": item["description"]
    }

with open("snippets.json", "w") as f:
    json.dump(result, f)