@echo off
echo Copying
copy FridayNightJazzin.j2l "D:\Games\Jazz Jackrabbit 2+\" /y
copy FridayNightJazzin.j2as "D:\Games\Jazz Jackrabbit 2+\" /y
echo Starting
"D:\Games\Jazz Jackrabbit 2+\Jazz2.exe" -server FridayNightJazzin
@echo on