@echo off
echo Copying
copy STVjetpack.mut "D:\Games\Jazz Jackrabbit 2+\" /y
echo Starting
"D:\Games\Jazz Jackrabbit 2+\Jazz2.exe" -server -mutators=STVjetpack.mut stvjetpack.j2l
@echo on