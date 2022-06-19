@echo off
echo Copying
copy STVtest1.mut "D:\Games\Jazz Jackrabbit 2+\" /y
echo Starting
"D:\Games\Jazz Jackrabbit 2+\Jazz2.exe" -server -mutators=STVtest1.mut -BATTLE
@echo on