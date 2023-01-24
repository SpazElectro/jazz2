@echo off
echo Copying
copy STVspyamongus.mut "F:\Games\Jazz2\" /y
echo Starting
"F:\Games\Jazz2\Jazz2+.exe" -server -mutators=STVspyamongus.mut battle1.j2l -BATTLE
@echo on