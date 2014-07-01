cmake_minimum_required(VERSION 2.8)

project(arcus)

add_definitions(-std=c++11)
set(CMAKE_BUILD_TYPE Debug)

add_library(arcus STATIC 
	Entity.cpp
	#BackEndPrint.cpp
	binders/JsonWrite.cpp
	binders/JsonRead.cpp
	RelationMap.cpp
	ErrorBinder.cpp
	ValidatorString.cpp
	)

include(../arcus.cmake)
