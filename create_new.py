from pick import pick
import os

ml = pick(["Mutator", "Level"], "Do you want to create a mutator or a level?", ">")
name = input("Name? ")

mutatorRun = """@echo off
echo Copying
copy \"../../scripts/\" \"F:\Games\Jazz2\\" /y
copy STV$N$.mut \"F:\Games\Jazz2\\" /y
echo Starting
"F:\Games\Jazz2\Jazz2+.exe" -server -mutators=STV$N$.mut battle1
@echo on"""

mutatorExample = """#pragma require \"STVutil.asc\"
#include \"STVutil.asc\"

void onLevelBegin() {
    jjConsole("Mutator started!");
}
"""

if ml[1] == 0:
    print(f"Creating new mutator named {name}...")
    os.mkdir(f"./mutators/{name}/")

    with open(f"./mutators/{name}/run.bat", "w") as r:
        mutatorRun = mutatorRun.replace("$N$", name)
        mutatorRun = mutatorRun.replace("$N$", name)
        r.write(mutatorRun)
    
    with open(f"./mutators/{name}/STV{name}.mut", "w") as m:
        m.write(mutatorExample)
else:
    print(f"Creating new level named {name}...")
