@echo off
echo Copying
copy "../../scripts/" "D:\Games\Jazz Jackrabbit 2+\" /y
copy teeworlds_tsf.j2t "D:\Games\Jazz Jackrabbit 2+\" /y
copy dm1.j2l "D:\Games\Jazz Jackrabbit 2+\" /y
copy dm1.j2as "D:\Games\Jazz Jackrabbit 2+\" /y
echo Starting
"D:\Games\Jazz Jackrabbit 2+\Jazz2.exe" -server dm1
@echo on