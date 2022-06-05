@echo off
echo Copying
copy STVteamhealth.mut "D:\Games\Jazz Jackrabbit 2+\" /y
echo Starting
"D:\Games\Jazz Jackrabbit 2+\Jazz2.exe" -server -mutators=STVteamhealth.mut face.j2l -ctf
@echo on