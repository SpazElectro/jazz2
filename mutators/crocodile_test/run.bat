@echo off
echo Copying
copy STVcrco.mut "D:\Games\Jazz Jackrabbit 2+\" /y
echo Starting
"D:\Games\Jazz Jackrabbit 2+\Jazz2.exe" -server -mutators=STVcrco.mut -battle
@echo on