import os
run = open(os.path.dirname(__file__) + "/config.ini").read()
parsed = run.split("\n")
runElements = {}

for t in parsed:
    runElements[t.split("=")[0]] = t.split("=")[1]

def getFromRun(index):
    return runElements[index]