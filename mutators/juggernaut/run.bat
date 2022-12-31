@echo off
echo Copying
copy STVjuggernaut.mut "C:\Game\Jazz Jackrabbit 2\createdbyme\" /y
echo Starting
"C:\Game\Jazz Jackrabbit 2\Jazz2.exe" -server -mutators=STVjuggernaut.mut
@echo on