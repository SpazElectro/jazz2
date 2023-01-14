@echo off
echo Copying
copy STVcharshop.mut "F:\Games\Jazz2\" /y
echo Starting
"F:\Games\Jazz2\Jazz2+.exe" -server -mutators=STVcharshop.mut
@echo on