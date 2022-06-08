@echo off
echo Copying
copy STVjuggernaut.mut "D:\Games\Jazz Jackrabbit 2+\" /y
echo Starting
"D:\Games\Jazz Jackrabbit 2+\Jazz2.exe" -server -mutators=STVjuggernaut.mut
@echo on