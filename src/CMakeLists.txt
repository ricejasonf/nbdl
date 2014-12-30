cmake_minimum_required(VERSION 2.8)

project(arcus)

add_definitions(-std=c++11)
set(CMAKE_BUILD_TYPE Debug)

add_library(arcus STATIC 
	binders/JsonRead.cpp
	binders/JsonCppApplyDiff.cpp
	Regex.cpp
	)

include(../arcus.cmake)
