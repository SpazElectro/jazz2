cmake_minimum_required(VERSION 3.0)
project(MyDLLProject)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libstdc++")
set(CMAKE_CXX_FLAGS "-m32")

set(SOURCES main.cpp)

add_library(MyDLL SHARED ${SOURCES})
