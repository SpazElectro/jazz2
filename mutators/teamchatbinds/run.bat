@echo off
echo Copying
copy STVteamchatbinds.mut "C:\Game\Jazz Jackrabbit 2\createdbyme\" /y
echo Starting
"C:\Game\Jazz Jackrabbit 2\Jazz2.exe" -server -mutators=STVteamchatbinds.mut face
@echo on