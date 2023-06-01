@echo off
cmake -B build -A Win32 .
cmake --build build
@echo on