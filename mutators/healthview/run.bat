@echo off
echo Copying
copy "../../scripts/" "F:\Games\Jazz2\" /y
copy STVhealthview.mut "F:\Games\Jazz2\" /y
echo Starting
"F:\Games\Jazz2\Jazz2+.exe" -server -mutators=STVhealthview.mut battle1
@echo on