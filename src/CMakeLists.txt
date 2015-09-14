cmake_minimum_required(VERSION 2.8)

project(nbdl)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

add_definitions(-std=c++11)

if (${EMSCRIPTEN})
	add_library(nbdl STATIC 
		Regex.cpp
		)
else (${EMSCRIPTEN})
	add_library(nbdl STATIC 
		binders/jsoncpp/Read.cpp
		Regex.cpp
		servers/ws/Parser.cpp
		)
endif (${EMSCRIPTEN})

include(../nbdl.cmake)
