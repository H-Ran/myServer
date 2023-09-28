cmake_minimum_required(VERSION 2.8)

set(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} ${CMAKE_CURRENT_SOURCE_DIR})
project(myServer LANGUAGES CXX)

message(Building All project.......)
set(CMAKE_CXX_FLAGS "-DEPOLL")
add_subdirectory(src/libs)
add_subdirectory(src/apps)
add_subdirectory(src/tools)