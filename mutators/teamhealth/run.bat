@echo off
echo Copying
copy STVteamhealth.mut "F:\Games\Jazz2\" /y
echo Starting
"F:\Games\Jazz2\Jazz2+.exe" -server -mutators=STVteamhealth.mut face.j2l -ctf
@echo on