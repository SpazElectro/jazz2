import os

configLocation = os.path.dirname(__file__) + "\\..\\config.ini"
if not os.path.exists(configLocation):
    gameDirectory = input("(ex: F:\Games\Jazz2) JJ2 game directory? ")
    gameName = input("(ex: Jazz2+.exe) JJ2 executable name? ")
    runFile = open(configLocation, "w")
    runFile.write(f"GAME_DIRECTORY={gameDirectory}\nGAME_NAME={gameName}")
    runFile.close()

runFile = open(configLocation)
run = runFile.read()
runFile.close()
parsed = run.split("\n")
runElements = {}

for t in parsed:
    runElements[t.split("=")[0]] = t.split("=")[1]

def getFromRun(index):
    return runElements[index]