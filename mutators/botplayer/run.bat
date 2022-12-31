@echo off
echo Copying
copy STVbotplayer.mut "C:\Game\Jazz Jackrabbit 2\createdbyme\" /y
echo Starting
"C:\Game\Jazz Jackrabbit 2\Jazz2.exe" -server -mutators=STVbotplayer.mut face.j2l -ctf
@echo on