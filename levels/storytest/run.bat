@echo off
echo Copying
copy "../../scripts/" "C:\Game\Jazz Jackrabbit 2\createdbyme\" /y
copy storytest.j2l "C:\Game\Jazz Jackrabbit 2\createdbyme\" /y
copy storytest.j2as "C:\Game\Jazz Jackrabbit 2\createdbyme\" /y
echo Starting
"C:\Game\Jazz Jackrabbit 2\Jazz2.exe" -server storytest
@echo on