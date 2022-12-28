@echo off
echo Copying
copy STVjuggernaut.mut "F:\Games\Jazz2\" /y
echo Starting
"F:\Games\Jazz2\Jazz2+.exe" -server -mutators=STVjuggernaut.mut
@echo on