@echo off
echo Copying
copy SpazLife.j2l "F:\Games\Jazz2\" /y
copy SpazLife.j2as "F:\Games\Jazz2\" /y
echo Starting
"F:\Games\Jazz2\Jazz2+.exe" -server SpazLife
@echo on