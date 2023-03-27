@echo off
echo Copying
copy "../../scripts/" "F:\Games\Jazz2\" /y
copy STVcustomweapons.mut "F:\Games\Jazz2\" /y
echo Starting
"F:\Games\Jazz2\Jazz2+.exe" -server -mutators=STVcustomweapons.mut battle1.j2l -battle
@echo on