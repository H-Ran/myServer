cmake_minimum_required(VERSION 2.8)

set(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} ${CMAKE_CURRENT_SOURCE_DIR})
project(myServer LANGUAGES CXX)

message(Building All project.......)

add_subdirectory(src/libs/libserver)
add_subdirectory(src/apps/login)
add_subdirectory(src/tools/robots)