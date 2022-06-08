@echo off
echo Copying
copy STVstartposviewer.mut "D:\Games\Jazz Jackrabbit 2+\" /y
echo Starting
"D:\Games\Jazz Jackrabbit 2+\Jazz2.exe" -server -mutators=STVstartposviewer.mut face
@echo on