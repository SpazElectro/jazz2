@echo off

REM Read variables from run.ini
for /f "tokens=1* delims==" %%a in ('type "..\..\run.ini" ^| find "="') do (
  if /i "%%a"=="GAME_DIRECTORY" set "GAME_DIRECTORY=%%b"
  if /i "%%a"=="GAME_NAME" set "GAME_NAME=%%b"
)

echo Copying files...
copy "../../scripts/" "%GAME_DIRECTORY%" /y

REM for %%i in (*.j2l *.j2t) do (
REM   copy "%%i" "%GAME_DIRECTORY%" /y
REM )
for %%i in (./assets/*.*) do (
  copy ".\assets\%%i" "%GAME_DIRECTORY%\STVhh24_%%i" /y
)
for %%i in (*.j2as *.mut *.asc) do (
  python ../../experiments/angelscriptpp/angelscriptpp.py "%%i" "%GAME_DIRECTORY%\%%i" -P "STVhh24"
)

set "J2L_LEVEL="
set "MUTATOR="

echo Starting
REM for %%i in (*.j2l) do (
REM   set "J2L_LEVEL=%%i"
REM   goto :check_mutator
REM )
set "J2L_LEVEL=stvhh24.j2l"

:check_mutator
for %%i in (*.mut) do (
  set "MUTATOR=-mutators=%%i"
  goto :start_game
)

:start_game
if not defined J2L_LEVEL set "J2L_LEVEL=battle1"
"%GAME_DIRECTORY%%GAME_NAME%" -server %MUTATOR% %J2L_LEVEL%
@echo on

