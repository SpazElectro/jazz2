@echo off
echo Copying
copy "../../scripts/" "D:\Games\Jazz Jackrabbit 2+\" /y
copy STVzombies.mut "D:\Games\Jazz Jackrabbit 2+\" /y
echo Starting
"D:\Games\Jazz Jackrabbit 2+\Jazz2.exe" -battle -mutators=STVzombies.mut -server face.j2l
@echo on