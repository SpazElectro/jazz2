codeInput = """items.insertLast(formatUInt(obj.points));
items.insertLast(formatUInt(obj.objectID));
items.insertLast(formatBoolToStringInteger(obj.scriptedCollisions));
items.insertLast(formatInt(obj.special));
items.insertLast(formatBoolToStringInteger(obj.triggersTNT));
items.insertLast(formatInt(obj.var[1]));
items.insertLast(formatInt(obj.var[2]));
items.insertLast(formatInt(obj.var[3]));
items.insertLast(formatInt(obj.var[4]));
items.insertLast(formatInt(obj.var[5]));
items.insertLast(formatInt(obj.var[6]));
items.insertLast(formatInt(obj.var[7]));
items.insertLast(formatInt(obj.var[8]));
items.insertLast(formatInt(obj.var[9]));
items.insertLast(formatInt(obj.var[10]));
items.insertLast(formatFloat(obj.xAcc));
items.insertLast(formatFloat(obj.xOrg));
items.insertLast(formatFloat(obj.xPos));
items.insertLast(formatFloat(obj.xSpeed));
items.insertLast(formatFloat(obj.yAcc));
items.insertLast(formatFloat(obj.yOrg));
items.insertLast(formatFloat(obj.yPos));
items.insertLast(formatFloat(obj.ySpeed));"""

output = ""

index = 23

for line in codeInput.split("\n"):
    output += "info.set(\"" + line.split("obj.")[1][:-3] + "\", parse" + line[23:].split("(")[0] + "(items[" + str(index) + "]));\n"
    index += 1

print(output)
