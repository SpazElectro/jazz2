@echo off
echo Copying
copy STVtest1.mut "F:\Games\Jazz2\" /y
echo Starting
"F:\Games\Jazz2\Jazz2+.exe" -server -mutators=STVtest1.mut -BATTLE
@echo on