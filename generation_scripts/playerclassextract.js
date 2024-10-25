let output = "";

// generate the {"property", player.property}
for (let child of children) {
    if(child.tagName == "DD") continue;
    if(child.childNodes.length != 2) continue;
    
    var type = child.childNodes[0].nodeValue.replace(/.$/, '');
    var thename = child.childNodes[1].innerHTML;

    if(type != null && thename != null)
        output += "{\"" + thename + "\", player." + thename + "},\n";
}

output += "\n\n";

// generate the if statements

for (let child of children) {
    if(child.tagName == "DD") continue;
    if(child.childNodes.length != 2) continue;
    
    var type = child.childNodes[0].nodeValue.replace(/.$/, '');
    var thename = child.childNodes[1].innerHTML;

    if(type != null && thename != null) {
        let ctype = type;
        if(ctype.startsWith("const"))
            ctype = ctype.split("const")[1];
        ctype = ctype.charAt(0).toUpperCase() + ctype.slice(1);
        if(ctype.charAt(0) == " ")
            ctype = ctype.substr(1);
        output += "                    if(key == \"" + thename + "\")\n                        checkAndExecuteCallback" + ctype + "(t, key, player." + thename + ", player);\n";
    }
}

output;