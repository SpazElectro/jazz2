from pick import pick
import os, shutil

ml = pick(["Mutator", "Level"], "Do you want to create a mutator or a level?", ">")
name = input("Name? ")

mutatorRun = """@echo off

REM Read variables from run.ini
for /f "tokens=1* delims==" %%a in ('type "..\..\\run.ini" ^| find "="') do (
  if /i "%%a"=="GAME_DIRECTORY" set "GAME_DIRECTORY=%%b"
  if /i "%%a"=="GAME_NAME" set "GAME_NAME=%%b"
)

echo Copying
copy "../../scripts/" "%GAME_DIRECTORY%" /y
copy STV$N$.mut "%GAME_DIRECTORY%" /y
echo Starting
"%GAME_DIRECTORY%%GAME_NAME%" -server -mutators=STV$N$.mut castle1
@echo on
"""

levelRun = """@echo off
REM Read variables from run.ini
for /f "tokens=1* delims==" %%a in ('type "..\..\\run.ini" ^| find "="') do (
  if /i "%%a"=="GAME_DIRECTORY" set "GAME_DIRECTORY=%%b"
  if /i "%%a"=="GAME_NAME" set "GAME_NAME=%%b"
)

echo Copying
copy "../../scripts/" "%GAME_DIRECTORY%" /y
copy STV$N$.j2l "%GAME_DIRECTORY%" /y
copy STV$N$.j2as "%GAME_DIRECTORY%" /y
echo Starting
"%GAME_DIRECTORY%%GAME_NAME%" -server STV$N$
@echo on
"""

exampleMutator = """#pragma name \"$N$\"
#pragma require \"STVutil.asc\"

#include \"STVutil.asc\"

// for intellisense
#include "../../scripts/STVutil.asc"

void onLevelBegin() {
    jjConsole("Started!");
}
"""

exampleLevel = """#pragma require \"STVutil.asc\"

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
        exampleMutator = exampleMutator.replace("$N$", name)
        m.write(exampleMutator)
else:
    print(f"Creating new level named {name}...")
    os.mkdir(f"./levels/{name}/")

    with open(f"./levels/{name}/run.bat", "w") as r:
        levelRun = levelRun.replace("$N$", name)
        levelRun = levelRun.replace("$N$", name)
        levelRun = levelRun.replace("$N$", name)
        r.write(levelRun)
    
    with open(f"./levels/{name}/STV{name}.j2as", "w") as m:
        m.write(exampleLevel)

    shutil.copy("./empty.j2l", f"./levels/{name}/STV{name}.j2l")
    
print("Finished!")