@echo off
echo Copying
copy "../../scripts/" "C:\Game\Jazz Jackrabbit 2\createdbyme\" /y
copy STVcustomweapons.mut "C:\Game\Jazz Jackrabbit 2\createdbyme\" /y
echo Starting
"C:\Game\Jazz Jackrabbit 2\Jazz2.exe" -server -mutators=STVcustomweapons.mut battle1.j2l -battle
@echo on