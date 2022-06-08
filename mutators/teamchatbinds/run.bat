@echo off
echo Copying
copy STVteamchatbinds.mut "D:\Games\Jazz Jackrabbit 2+\" /y
echo Starting
"D:\Games\Jazz Jackrabbit 2+\Jazz2.exe" -server -mutators=STVteamchatbinds.mut face
@echo on