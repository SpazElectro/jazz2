cmake_minimum_required(VERSION 3.0)
project(funccall)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(funccall SHARED main.cpp)

set_target_properties(funccall PROPERTIES OUTPUT_NAME "funccall")
