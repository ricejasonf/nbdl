cmake_minimum_required(VERSION 2.8)

project(arcus)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

add_definitions(-std=c++11)

add_library(arcus STATIC 
	#binders/JsonRead.cpp
	#binders/JsonCppApplyDiff.cpp
	Regex.cpp
	)

include(../arcus.cmake)
