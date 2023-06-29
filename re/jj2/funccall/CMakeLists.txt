cmake_minimum_required(VERSION 3.0)
project(funccall)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_subdirectory(3rdparty)
add_library(funccall SHARED main.cpp)
target_link_libraries(funccall PRIVATE lib_detours)
set_target_properties(funccall PROPERTIES LINK_FLAGS /EXPORT:DetourFinishHelperProcess,@1,NONAME)
set_target_properties(funccall PROPERTIES OUTPUT_NAME "funccall")
