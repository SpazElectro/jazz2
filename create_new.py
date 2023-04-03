from pick import pick
import os, shutil

ml = pick(["Mutator", "Level"], "Do you want to create a mutator or a level?", ">")
name = input("Name? ")

mutatorRun = """@echo off
echo Copying
copy \"../../scripts/\" \"F:\\Games\\Jazz2\\" /y
copy STV$N$.mut \"F:\\Games\\Jazz2\\" /y
echo Starting
"F:\\Games\\Jazz2\\Jazz2+.exe" -server -mutators=STV$N$.mut battle1
@echo on"""

levelRun = """@echo off
echo Copying
copy "../../scripts/" "F:\\Games\\Jazz2\\" /y
copy STV$N$.j2l "F:\\Games\\Jazz2\\" /y
copy STV$N$.j2as "F:\\Games\\Jazz2\\" /y
echo Starting
"F:\\Games\\Jazz2\\Jazz2+.exe" -server STV$N$
@echo on"""

example = """#pragma name \"$N$\"
#pragma require \"STVutil.asc\"

#include \"STVutil.asc\"

// for intellisense
#include "../../scripts/STVutil.asc"

void onLevelBegin() {
    jjConsole("Started!");
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
        example = example.replace("$N$", name)
        m.write(example)
else:
    print(f"Creating new level named {name}...")
    os.mkdir(f"./levels/{name}/")

    with open(f"./levels/{name}/run.bat", "w") as r:
        levelRun = levelRun.replace("$N$", name)
        levelRun = levelRun.replace("$N$", name)
        levelRun = levelRun.replace("$N$", name)
        r.write(levelRun)
    
    with open(f"./levels/{name}/STV{name}.j2as", "w") as m:
        m.write(example)

    shutil.copy("./empty.j2l", f"./levels/{name}/STV{name}.j2l")
    
print("Finished!")