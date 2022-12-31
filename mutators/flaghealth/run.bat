@echo off
echo Copying
copy STVflaghealth.mut "C:\Game\Jazz Jackrabbit 2\createdbyme\" /y
echo Starting
"C:\Game\Jazz Jackrabbit 2\Jazz2.exe" -server -mutators=STVflaghealth.mut face.j2l -ctf
@echo on