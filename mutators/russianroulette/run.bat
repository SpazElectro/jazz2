@echo off
echo Copying
copy STVrussianroulette.mut "D:\Games\Jazz Jackrabbit 2+\" /y
echo Starting
"D:\Games\Jazz Jackrabbit 2+\Jazz2.exe" -server -mutators=STVrussianroulette.mut
@echo on