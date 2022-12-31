@echo off
echo Copying
copy FridayNightJazzin.j2l "C:\Game\Jazz Jackrabbit 2\createdbyme\" /y
copy FridayNightJazzin.j2as "C:\Game\Jazz Jackrabbit 2\createdbyme\" /y
echo Starting
"C:\Game\Jazz Jackrabbit 2\Jazz2.exe" -server FridayNightJazzin
@echo on