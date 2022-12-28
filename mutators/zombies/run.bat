@echo off
echo Copying
copy "../../scripts/" "F:\Games\Jazz2\" /y
copy STVzombies.mut "F:\Games\Jazz2\" /y
echo Starting
"F:\Games\Jazz2\Jazz2+.exe" -battle -mutators=STVzombies.mut -server face.j2l
@echo on