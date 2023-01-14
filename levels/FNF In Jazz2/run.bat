@echo off
echo Copying
copy FridayNightJazzin.j2l "F:\Games\Jazz2\" /y
copy FridayNightJazzin.j2as "F:\Games\Jazz2\" /y
echo Starting
"F:\Games\Jazz2\Jazz2+.exe" -server FridayNightJazzin
@echo on