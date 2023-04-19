@echo off
echo Starting

REM Read variables from run.ini
for /f "tokens=1* delims==" %%a in ('type "..\..\run.ini" ^| find "="') do (
  if /i "%%a"=="GAME_DIRECTORY" set "GAME_DIRECTORY=%%b"
  if /i "%%a"=="GAME_NAME" set "GAME_NAME=%%b"
)

"%GAME_DIRECTORY%%GAME_NAME%" -connect 127.0.0.1
@echo on