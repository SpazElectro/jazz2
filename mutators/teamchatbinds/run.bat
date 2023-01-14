@echo off
echo Copying
copy STVteamchatbinds.mut "F:\Games\Jazz2\" /y
echo Starting
"F:\Games\Jazz2\Jazz2+.exe" -server -mutators=STVteamchatbinds.mut face
@echo on