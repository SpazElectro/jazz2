@echo off
echo Copying
copy mainmenuplus.j2l "C:\Game\Jazz Jackrabbit 2\createdbyme\" /y
copy mainmenuplus.j2as "C:\Game\Jazz Jackrabbit 2\createdbyme\" /y
echo Starting
"F:\Games\Jazz2\Jazz2.exe" -server mainmenuplus
@echo on