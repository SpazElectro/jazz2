@echo off
echo Copying
copy SpazLife.j2l "C:\Game\Jazz Jackrabbit 2\createdbyme\" /y
copy SpazLife.j2as "C:\Game\Jazz Jackrabbit 2\createdbyme\" /y
echo Starting
"C:\Game\Jazz Jackrabbit 2\Jazz2.exe" -server SpazLife
@echo on