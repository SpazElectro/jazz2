@echo off
echo Copying
copy STVcharshop.mut "C:\Game\Jazz Jackrabbit 2\createdbyme\" /y
echo Starting
"C:\Game\Jazz Jackrabbit 2\Jazz2.exe" -server -mutators=STVcharshop.mut
@echo on