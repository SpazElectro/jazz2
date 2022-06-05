@echo off
echo Copying
copy STVflaghealth.mut "D:\Games\Jazz Jackrabbit 2+\" /y
echo Starting
"D:\Games\Jazz Jackrabbit 2+\Jazz2.exe" -server -mutators=STVflaghealth.mut face.j2l -ctf
@echo on