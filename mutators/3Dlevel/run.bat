@echo off
echo Copying
copy 3Dlevel.mut "F:\Games\Jazz2\" /y
echo Starting
"F:\Games\Jazz2\Jazz2+.exe" -server -mutators=3Dlevel.mut -battle
@echo on