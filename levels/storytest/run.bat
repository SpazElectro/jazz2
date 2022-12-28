@echo off
echo Copying
copy "../../scripts/" "F:\Games\Jazz2\" /y
copy storytest.j2l "F:\Games\Jazz2\" /y
copy storytest.j2as "F:\Games\Jazz2\" /y
echo Starting
"F:\Games\Jazz2\Jazz2+.exe" -server storytest
@echo on