@echo off
echo Copying
copy "../../scripts/" "F:\Games\Jazz2\" /y
copy STVmansion.j2l "F:\Games\Jazz2\" /y
copy STVmansion.j2as "F:\Games\Jazz2\" /y
copy STVmansion_dialogue.asc "F:\Games\Jazz2\" /y
cd assets
copy STVMansion_test.png "F:\Games\Jazz2\" /y
copy STVMansion_test2.png "F:\Games\Jazz2\" /y
cd ../
echo Starting
"F:\Games\Jazz2\Jazz2+.exe" -server STVmansion
@echo on