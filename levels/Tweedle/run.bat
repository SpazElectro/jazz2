@echo off
echo Copying
copy STVtweedle.j2l "D:\Games\Jazz Jackrabbit 2+\" /y
copy STVtweedle.j2as "D:\Games\Jazz Jackrabbit 2+\" /y
echo Starting
"D:\Games\Jazz Jackrabbit 2+\Jazz2.exe" -server STVtweedle
@echo on