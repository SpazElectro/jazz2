@echo off
echo Copying
copy STVlaserbeam.mut "F:\Games\Jazz2\" /y
echo Starting
"F:\Games\Jazz2\Jazz2+.exe" -server -mutators=STVlaserbeam.mut battle1
@echo on