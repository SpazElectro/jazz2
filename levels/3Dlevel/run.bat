@echo off
echo Copying
copy 3Dlevel.mut "C:\Game\Jazz Jackrabbit 2\createdbyme\" /y
echo Starting
"C:\Game\Jazz Jackrabbit 2\Jazz2.exe" -server -mutators=3Dlevel.mut -battle
@echo on