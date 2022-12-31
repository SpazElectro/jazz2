@echo off
echo Copying
copy STVstartposviewer.mut "C:\Game\Jazz Jackrabbit 2\createdbyme\" /y
echo Starting
"C:\Game\Jazz Jackrabbit 2\Jazz2.exe" -server -mutators=STVstartposviewer.mut face
@echo on