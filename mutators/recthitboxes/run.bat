@echo off
echo Copying
copy STVrecthitboxes.mut "F:\Games\Jazz2\" /y
echo Starting
"F:\Games\Jazz2\Jazz2+.exe" -server -mutators=STVrecthitboxes.mut battle1
@echo on