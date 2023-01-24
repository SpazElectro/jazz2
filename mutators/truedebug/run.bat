@echo off
echo Copying
copy "../../scripts/" "F:\Games\Jazz2\" /y
copy STVtruedebug.mut "F:\Games\Jazz2\" /y
echo Starting
"F:\Games\Jazz2\Jazz2+.exe" -server -mutators=STVtruedebug.mut -BATTLE
@echo on