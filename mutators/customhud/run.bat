@echo off
echo Copying
copy "../../scripts/" "F:\Games\Jazz2\" /y
copy STVcustomhud.mut "F:\Games\Jazz2\" /y
echo Starting
"F:\Games\Jazz2\Jazz2+.exe" -server -mutators=STVcustomhud.mut castle1
@echo on