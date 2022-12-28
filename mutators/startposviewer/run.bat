@echo off
echo Copying
copy STVstartposviewer.mut "F:\Games\Jazz2\" /y
echo Starting
"F:\Games\Jazz2\Jazz2+.exe" -server -mutators=STVstartposviewer.mut face
@echo on