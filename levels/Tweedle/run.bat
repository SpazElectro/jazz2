@echo off
echo Copying
copy STVtweedle.j2l "C:\Game\Jazz Jackrabbit 2\createdbyme\" /y
copy STVtweedle.j2as "C:\Game\Jazz Jackrabbit 2\createdbyme\" /y
echo Starting
"C:\Game\Jazz Jackrabbit 2\Jazz2.exe" -server STVtweedle
@echo on