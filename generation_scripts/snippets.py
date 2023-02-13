# abc = document.getElementById("abc")
# var items = []
# var prevDescription = '';

# function findNextDD(element, startIndex) {
#   let resultx = "";
#   let counter = 0;

#   for (let i = startIndex; i < element.children.length; i++) {
#     if (element.children[i].tagName === "DD") {
#       resultx += element.children[i].textContent + "\n";
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
#     var full = "";

#     for (let it = 0; it < abc.children[i].children.length; it++) {
#         if(abc.children[i].nodeType == 3) {
#             full += abc.children[i].nodeValue
#         } else {
#             full += abc.children[i].innerText
#         }

#         if(full.includes(")")) break;
#     }
    
#     if(name == "") continue;

#     items.push({
#       name: name,
#       description: description,
#       full: full
#     });
#   }
# }

# console.log(items)

import json

result = {}

for item in json.load(open("api.json")):
    result[item["name"]] = {
        "prefix": item["name"],
        "body": item["name"],
        "description": item["description"]
    }

for item in json.load(open("apifunctions.json")):
    result[item["name"]] = {
        "prefix": item["name"],
        "body": item["full"],
        "description": item["description"]
    }

with open("snippets_build.json", "w") as f:
    json.dump(result, f)
