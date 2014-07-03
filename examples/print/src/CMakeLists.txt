cmake_minimum_required(VERSION 2.8)

project(print)

add_definitions(-std=c++11)
set(CMAKE_BUILD_TYPE Debug)

add_executable(print
	print.cpp
	Account.cpp
	Address.cpp
	Food.cpp
	FoodGroup.cpp
	)

SET(LIBARCUS_DIR
	/home/jason/Projects/NewHotness
	)
include_directories(${LIBARCUS_DIR}/src)
include(${LIBARCUS_DIR}/arcus.cmake)
target_link_libraries(print ${ARCUS_LIB_DIR}/libarcus.a)
target_link_libraries(print ${Boost_REGEX_LIBRARY})
target_link_libraries(print ${JsonCpp})
