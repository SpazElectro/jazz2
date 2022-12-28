@echo off
echo Copying
copy STVjetpack.mut "F:\Games\Jazz2\" /y
echo Starting
"F:\Games\Jazz2\Jazz2+.exe" -server -mutators=STVjetpack.mut stvjetpack.j2l
@echo on