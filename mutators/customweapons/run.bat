@echo off
echo Copying
copy "../../scripts/" "D:\Games\Jazz Jackrabbit 2+\" /y
copy STVcustomweapons.mut "D:\Games\Jazz Jackrabbit 2+\" /y
echo Starting
"D:\Games\Jazz Jackrabbit 2+\Jazz2.exe" -server -mutators=STVcustomweapons.mut battle1.j2l -battle
@echo on