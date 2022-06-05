@echo off
echo Copying
copy STVweaponlist.mut "D:\Games\Jazz Jackrabbit 2+\" /y
echo Starting
"D:\Games\Jazz Jackrabbit 2+\Jazz2.exe" -server -mutators=STVweaponlist.mut
@echo on