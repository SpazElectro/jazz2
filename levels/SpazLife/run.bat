@echo off
echo Copying
copy SpazLife.j2l "D:\Games\Jazz Jackrabbit 2+\" /y
copy SpazLife.j2as "D:\Games\Jazz Jackrabbit 2+\" /y
echo Starting
"D:\Games\Jazz Jackrabbit 2+\Jazz2.exe" -server SpazLife
@echo on