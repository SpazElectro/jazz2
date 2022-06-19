@echo off
echo Copying
copy breakingpoint.j2as "D:\Games\Jazz Jackrabbit 2+\" /y
copy breakingpoint.j2l "D:\Games\Jazz Jackrabbit 2+\" /y
echo Starting
"D:\Games\Jazz Jackrabbit 2+\Jazz2.exe" -server -LRS breakingpoint
@echo on