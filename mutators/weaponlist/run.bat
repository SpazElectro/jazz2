@echo off
echo Copying
copy STVweaponlist.mut "F:\Games\Jazz2\" /y
copy "../../scripts/" "F:\Games\Jazz2\" /y
echo Starting
"F:\Games\Jazz2\Jazz2+.exe" -server -mutators=STVweaponlist.mut
@echo on