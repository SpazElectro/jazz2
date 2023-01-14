@echo off
echo Copying
copy STVbotplayer.mut "F:\Games\Jazz2\" /y
echo Starting
"F:\Games\Jazz2\Jazz2+.exe" -server -mutators=STVbotplayer.mut face.j2l -ctf
@echo on