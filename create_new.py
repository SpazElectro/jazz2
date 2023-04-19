from pick import pick
import os, shutil

ml = pick(["Mutator", "Level"], "Do you want to create a mutator or a level?", ">")
name = input("Name? ")

run = """@echo off

REM Read variables from run.ini
for /f "tokens=1* delims==" %%a in ('type "..\..\run.ini" ^| find "="') do (
  if /i "%%a"=="GAME_DIRECTORY" set "GAME_DIRECTORY=%%b"
  if /i "%%a"=="GAME_NAME" set "GAME_NAME=%%b"
)

echo Copying files...
copy "../../scripts/" "%GAME_DIRECTORY%" /y
for %%i in (*.j2l *.j2as *.mut) do (
  copy "%%i" "%GAME_DIRECTORY%" /y
)

set "J2L_LEVEL="
set "MUTATOR="

echo Starting
for %%i in (*.j2l) do (
  set "J2L_LEVEL=%%i"
  goto :check_mutator
)

:check_mutator
for %%i in (*.mut) do (
  set "MUTATOR=-mutators=%%i"
  goto :start_game
)

:start_game
if not defined J2L_LEVEL set "J2L_LEVEL=battle1"
"%GAME_DIRECTORY%%GAME_NAME%" -server %MUTATOR% %J2L_LEVEL% -battle
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
        r.write(run)
    
    with open(f"./mutators/{name}/STV{name}.mut", "w") as m:
        exampleMutator = exampleMutator.replace("$N$", name)
        m.write(exampleMutator)
else:
    print(f"Creating new level named {name}...")
    os.mkdir(f"./levels/{name}/")

    with open(f"./levels/{name}/run.bat", "w") as r:
        r.write(run)
    
    with open(f"./levels/{name}/STV{name}.j2as", "w") as m:
        m.write(exampleLevel)

    shutil.copy("./empty.j2l", f"./levels/{name}/STV{name}.j2l")
    
print("Finished!")