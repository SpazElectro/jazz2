@echo off
@REM pyinstaller main.py
copy classes.json dist\main\_internal\classes.json /Y
copy global.json dist\main\_internal\global.json /Y
xcopy /E /I /Y dist\main\ .\bundle\spazlint\
cd vsc-extension
cmd /c vsce package -o "..\bundle\extension.vsix"
cd ../
7z a -tzip "dist.zip" .\bundle
@echo on