cmake_minimum_required(VERSION 2.8)

project(arcus)

add_definitions(-std=c++11)

add_library(arcus STATIC 
	Entity.cpp
	Validator.cpp
	BackEndPrint.cpp
	)

include(../arcus.cmake)
