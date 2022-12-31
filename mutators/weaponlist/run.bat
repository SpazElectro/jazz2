@echo off
echo Copying
copy STVweaponlist.mut "C:\Game\Jazz Jackrabbit 2\createdbyme\" /y
copy STVweaponlist_constants.asc "C:\Game\Jazz Jackrabbit 2\createdbyme\" /y
echo Starting
"C:\Game\Jazz Jackrabbit 2\Jazz2.exe" -server -mutators=STVweaponlist.mut
@echo on