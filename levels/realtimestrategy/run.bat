@echo off
echo Copying
copy "../../scripts/" "F:\Games\Jazz2\" /y
copy STVrealtimestrategy.j2l "F:\Games\Jazz2\" /y
copy STVrealtimestrategy.j2as "F:\Games\Jazz2\" /y
echo Starting
"F:\Games\Jazz2\Jazz2+.exe" -server STVrealtimestrategy
@echo on