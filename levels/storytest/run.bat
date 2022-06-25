@echo off
echo Copying
copy "../../scripts/" "D:\Games\Jazz Jackrabbit 2+\" /y
copy storytest.j2l "D:\Games\Jazz Jackrabbit 2+\" /y
copy storytest.j2as "D:\Games\Jazz Jackrabbit 2+\" /y
echo Starting
"D:\Games\Jazz Jackrabbit 2+\Jazz2.exe" -server storytest
@echo on