@echo off
echo Copying
copy STVbotplayer.mut "D:\Games\Jazz Jackrabbit 2+\" /y
echo Starting
"D:\Games\Jazz Jackrabbit 2+\Jazz2.exe" -server -mutators=STVbotplayer.mut face.j2l -ctf
@echo on