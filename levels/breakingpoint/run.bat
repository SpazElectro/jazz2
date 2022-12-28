@echo off
echo Copying
copy breakingpoint.j2as "F:\Games\Jazz2\" /y
copy breakingpoint.j2l "F:\Games\Jazz2\" /y
echo Starting
"F:\Games\Jazz2\Jazz2+.exe" -server -LRS breakingpoint
@echo on