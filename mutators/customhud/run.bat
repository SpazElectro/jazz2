@echo off

REM Read variables from run.ini
for /f "tokens=1* delims==" %%a in ('type "../../run.ini" ^| find "="') do (
  if /i "%%a"=="GAME_DIRECTORY" set "GAME_DIRECTORY=%%b"
  if /i "%%a"=="GAME_NAME" set "GAME_NAME=%%b"
)

echo Copying
copy "../../scripts/" "%GAME_DIRECTORY%" /y
copy STVcustomhud.mut "%GAME_DIRECTORY%" /y
echo Starting
"%GAME_DIRECTORY%%GAME_NAME%" -server -mutators=STVcustomhud.mut castle1
@echo on
