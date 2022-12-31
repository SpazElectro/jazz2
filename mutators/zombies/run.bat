@echo off
echo Copying
copy "../../scripts/" "C:\Game\Jazz Jackrabbit 2\createdbyme\" /y
copy STVzombies.mut "C:\Game\Jazz Jackrabbit 2\createdbyme\" /y
echo Starting
"C:\Game\Jazz Jackrabbit 2\Jazz2.exe" -battle -mutators=STVzombies.mut -server face.j2l
@echo on