@echo off

REM Read variables from run.ini
for /f "tokens=1* delims==" %%a in ('type "..\..\run.ini" ^| find "="') do (
  if /i "%%a"=="GAME_DIRECTORY" set "GAME_DIRECTORY=%%b"
  if /i "%%a"=="GAME_NAME" set "GAME_NAME=%%b"
)

REM Close any running instances
taskkill /f /im jazz2.exe

echo Copying files...
copy "../../scripts/" "%GAME_DIRECTORY%" /y

for %%i in (*.j2l *.j2t) do (
  copy "%%i" "%GAME_DIRECTORY%" /y
)
for %%i in (./assets/*.*) do (
  copy ".\assets\%%i" "%GAME_DIRECTORY%\STVmotorracer_%%i" /y
)
for %%i in (*.j2as *.mut *.asc) do (
  python ../../experiments/angelscriptpp/angelscriptpp.py "%%i" "%GAME_DIRECTORY%\%%i" -P "motorracer"
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
"%GAME_DIRECTORY%%GAME_NAME%" -server %MUTATOR% %J2L_LEVEL%
@echo on

