@echo off
echo Copying
copy STVteamhealth.mut "C:\Game\Jazz Jackrabbit 2\createdbyme\" /y
echo Starting
"C:\Game\Jazz Jackrabbit 2\Jazz2.exe" -server -mutators=STVteamhealth.mut face.j2l -ctf
@echo on