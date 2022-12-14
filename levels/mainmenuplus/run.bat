@echo off
echo Copying
copy mainmenuplus.j2l "F:\Games\Jazz2\" /y
copy mainmenuplus.j2as "F:\Games\Jazz2\" /y
echo Starting
"F:\Games\Jazz2\Jazz2.exe" -server mainmenuplus
@echo on