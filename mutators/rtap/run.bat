@echo off
echo Copying
copy "../../scripts/" "F:\Games\Jazz2\" /y
copy STVrtap.mut "F:\Games\Jazz2\" /y
copy rtaptest.j2l "F:\Games\Jazz2\" /y
echo Starting
"F:\Games\Jazz2\Jazz2+.exe" -server -mutators=STVrtap.mut rtaptest
@echo on