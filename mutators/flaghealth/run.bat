@echo off
echo Copying
copy STVflaghealth.mut "F:\Games\Jazz2\" /y
echo Starting
"F:\Games\Jazz2\Jazz2+.exe" -server -mutators=STVflaghealth.mut face.j2l -ctf
@echo on