@echo off
echo Copying
copy STVlaserbeam.mut "C:\Game\Jazz Jackrabbit 2\createdbyme\" /y
echo Starting
"C:\Game\Jazz Jackrabbit 2\Jazz2.exe" -server -mutators=STVlaserbeam.mut battle1
@echo on