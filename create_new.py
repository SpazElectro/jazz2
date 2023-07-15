from pick import pick
import os, shutil

ml = pick(["Mutator", "Level"], "Do you want to create a mutator or a level?", ">")
name = input("Name? ")
createAssets = pick(["No", "Yes"], "Assets? (y/n) ")

assetsRun = """@echo off

REM Read variables from run.ini
for /f "tokens=1* delims==" %%a in ('type "..\..\\run.ini" ^| find "="') do (
  if /i "%%a"=="GAME_DIRECTORY" set "GAME_DIRECTORY=%%b"
  if /i "%%a"=="GAME_NAME" set "GAME_NAME=%%b"
)

echo Copying files...
copy "../../scripts/" "%GAME_DIRECTORY%" /y
for %%i in (*.j2l *.j2as *.mut *.asc) do (
  copy "%%i" "%GAME_DIRECTORY%" /y
)
for %%i in (./assets/*.*) do (
  copy ".\\assets\%%i" "%GAME_DIRECTORY%" /y
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

run = """@echo off

REM Read variables from run.ini
for /f "tokens=1* delims==" %%a in ('type "..\..\\run.ini" ^| find "="') do (
  if /i "%%a"=="GAME_DIRECTORY" set "GAME_DIRECTORY=%%b"
  if /i "%%a"=="GAME_NAME" set "GAME_NAME=%%b"
)

echo Copying files...
copy "../../scripts/" "%GAME_DIRECTORY%" /y
for %%i in (*.j2l *.j2as *.mut *.asc) do (
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

directory = ("./mutators/" if ml[1] == 0 else "./levels/") + name

print(f"Creating new {'mutator' if ml[1] == 0 else 'level'} named {name}...")
os.mkdir(directory)

if createAssets[1] == 1:
  os.mkdir(f"{directory}/assets/")
  run = assetsRun

with open(f"{directory}/run.bat", "w") as r:
    r.write(run)

with open(f"{directory}/STV{name}.{'mut' if ml[1] == 0 else 'j2as'}", "w") as f:
    if ml[1] == 0:
        f.write(exampleMutator.replace("$N$", name))
    else:
        f.write("\n".join(exampleMutator.split("\n")[1:]))

if not ml[1] == 0:  
  shutil.copy("./empty.j2l", f"./levels/{name}/STV{name}.j2l")  

print("Finished!")