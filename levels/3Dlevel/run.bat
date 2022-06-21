@echo off
echo Copying
copy 3Dlevel.mut "D:\Games\Jazz Jackrabbit 2+\" /y
echo Starting
"D:\Games\Jazz Jackrabbit 2+\Jazz2.exe" -server -mutators=3Dlevel.mut -battle
@echo on