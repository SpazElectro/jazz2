codeInput = """info.set("age", parseInt(items[0]));
info.set("animSpeed", parseInt(items[1]));
info.set("causesRicochet", parseBool(items[2]));
info.set("counter", parseInt(items[3]));
info.set("counterEnd", parseUInt(items[4]));
info.set("creator", parseInt(items[5]));
info.set("creatorID", parseInt(items[6]));
info.set("curAnim", parseInt(items[7]));
info.set("curFrame", parseInt(items[8]));
info.set("deactivates", parseBool(items[9]));
info.set("direction", parseInt(items[10]));
info.set("doesHurt", parseUInt(items[11]));
info.set("energy", parseInt(items[12]));
info.set("eventID", parseUInt(items[13]));
info.set("frameID", parseInt(items[14]));
info.set("freeze", parseUInt(items[15]));
info.set("isActive", parseBool(items[16]));
info.set("isBlastable", parseBool(items[17]));
info.set("isFreezable", parseBool(items[18]));
info.set("isTarget", parseBool(items[19]));
info.set("justHit", parseUInt(items[20]));
info.set("killAnim", parseInt(items[21]));
info.set("light", parseInt(items[22]));
info.set("points", parseUInt(items[23]));
info.set("objectID", parseUInt(items[24]));
info.set("scriptedCollisions", parseBool(items[25]));
info.set("special", parseInt(items[26]));
info.set("triggersTNT", parseBool(items[27]));
info.set("var[1]", parseInt(items[28]));
info.set("var[2]", parseInt(items[29]));
info.set("var[3]", parseInt(items[30]));
info.set("var[4]", parseInt(items[31]));
info.set("var[5]", parseInt(items[32]));
info.set("var[6]", parseInt(items[33]));
info.set("var[7]", parseInt(items[34]));
info.set("var[8]", parseInt(items[35]));
info.set("var[9]", parseInt(items[36]));
info.set("var[10]", parseInt(items[37]));
info.set("xAcc", parseFloat(items[38]));
info.set("xOrg", parseFloat(items[39]));
info.set("xPos", parseFloat(items[40]));
info.set("xSpeed", parseFloat(items[41]));
info.set("yAcc", parseFloat(items[42]));
info.set("yOrg", parseFloat(items[43]));
info.set("yPos", parseFloat(items[44]));
info.set("ySpeed", parseFloat(items[45]));"""

output = ""

for line in codeInput.split("\n"):
    itemType = line.split(", ")[1].split("(")[0].split("parse")[1].lower()

    output += "object." + line.split("\"")[1] + " = " + itemType + "(info[\"" + line.split("\"")[1] + "\"]);\n"

print(output)
