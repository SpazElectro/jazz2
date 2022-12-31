@echo off
echo Copying
copy STVjetpack.mut "C:\Game\Jazz Jackrabbit 2\createdbyme\" /y
echo Starting
"C:\Game\Jazz Jackrabbit 2\Jazz2.exe" -server -mutators=STVjetpack.mut stvjetpack.j2l
@echo on