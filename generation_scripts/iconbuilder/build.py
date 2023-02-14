from random import randint

import os, shutil

animationSettings = """default:
        coldspot: 0,0
        fps: 10
        gunspot: 0,0
        origin: 0,0
        tagged: 0"""

frameId = 0

try: shutil.rmtree("./j2a/icons-j2a")
except Exception: pass

os.mkdir("./j2a/icons-j2a")
os.mkdir("./j2a/icons-j2a/0")
os.mkdir("./j2a/icons-j2a/0/0")

xid = 0

mainIndex = 0
subIndex = 0

with open(f"./j2a/icons-j2a/{mainIndex}/{subIndex}/animation.settings", "w") as animsettings:
    animsettings.write(animationSettings)
    animsettings.close()

for x in os.listdir("../../icons/"):
    shutil.copyfile(f"../../icons/{x}", f"./j2a/icons-j2a/{mainIndex}/{subIndex}/frame-{xid:03}.png")
    
    print(f"Built {x} in icons-j2a/{mainIndex}/{subIndex}/frame-{xid:03}")

    xid += 1

    if xid >= 1000:
        subIndex += 1
        xid = 0
        os.mkdir(f"./j2a/icons-j2a/{mainIndex}/{subIndex}")
        
        with open(f"./j2a/icons-j2a/{mainIndex}/{subIndex}/animation.settings", "w") as animsettings:
            animsettings.write(animationSettings)
            animsettings.close()
    if subIndex >= 100:
        mainIndex += 1
        subIndex = 0
        xid = 0
        os.mkdir(f"./j2a/icons-j2a/{mainIndex}/{subIndex}")

os.system("python ./j2a/j2a-import.py \"./j2a/icons-j2a\"")

prepareInSD = input("Do you want to open JazzSD with this animation file? (Y/n)")

if prepareInSD == "" or prepareInSD.lower() == "y":
    if os.path.exists("../../sprites/Anims.j2a"):
        os.rename("../../sprites/Anims.j2a", "../../sprites/cache/Anims.j2a.old-" + str(randint(1, 10000)))
    
    shutil.copyfile("./j2a/icons.j2a", "../../sprites/Anims.j2a")

    os.chdir("../../sprites/")
    os.startfile("JazzSD.exe")
