@echo off
echo Copying
copy STVtweedle.j2l "F:\Games\Jazz2\" /y
copy STVtweedle.j2as "F:\Games\Jazz2\" /y
echo Starting
"F:\Games\Jazz2\Jazz2+.exe" -server STVtweedle
@echo on