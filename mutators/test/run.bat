@echo off
echo Copying
copy STVtest1.mut "C:\Game\Jazz Jackrabbit 2\createdbyme\" /y
echo Starting
"C:\Game\Jazz Jackrabbit 2\Jazz2.exe" -server -mutators=STVtest1.mut -BATTLE
@echo on