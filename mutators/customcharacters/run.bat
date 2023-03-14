@echo off
echo Copying
copy "../../scripts/" "F:\Games\Jazz2\" /y
copy STVcustomcharacters.mut "F:\Games\Jazz2\" /y
echo Starting
"F:\Games\Jazz2\Jazz2+.exe" -server -mutators=STVcustomcharacters.mut battle1
@echo on