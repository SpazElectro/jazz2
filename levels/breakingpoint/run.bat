@echo off
echo Copying
copy breakingpoint.j2as "C:\Game\Jazz Jackrabbit 2\createdbyme\" /y
copy breakingpoint.j2l "C:\Game\Jazz Jackrabbit 2\createdbyme\" /y
echo Starting
"C:\Game\Jazz Jackrabbit 2\Jazz2.exe" -server -LRS breakingpoint
@echo on