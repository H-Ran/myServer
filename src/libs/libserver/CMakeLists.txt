cmake_minimum_required(VERSION 2.8)

if (CMAKE_BUILD_TYPE STREQUAL Release) 
	set(MyProjectName server)
else()
	set(CMAKE_BUILD_TYPE Debug)
	set(MyProjectName serverd)
endif()

message(STATUS "CMAKE_BUILD_TYPE:${CMAKE_BUILD_TYPE}")
message(STATUS "ProjectName:${MyProjectName}")

project(${MyProjectName})

include_directories(/usr/local/include)


aux_source_directory(. SRCS)
aux_source_directory(./protobuf SRCS)


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "../../../libs")

set(CMAKE_CXX_FLAGS "-Wall  -std=c++11 -pthread -lprotobuf  -DEPOLL")
set(CMAKE_CXX_FLAGS_DEBUG "-D_DEBUG -O0 -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")



add_library(${MyProjectName} STATIC ${SRCS})

target_link_libraries(${MyProjectName} PUBLIC /usr/local/lib/libprotobuf.a)