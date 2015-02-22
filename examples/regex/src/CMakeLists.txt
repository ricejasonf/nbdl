cmake_minimum_required(VERSION 2.8)

project(regex)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

add_definitions(-std=c++11)

add_executable(regex
	main.cpp
	)

SET(LIBARCUS_DIR
	/home/jason/Projects/NewHotness
	)
include_directories(${LIBARCUS_DIR}/src)
include(${LIBARCUS_DIR}/arcus.cmake)
target_link_libraries(regex ${ARCUS_LIB_DIR}/libarcus.a)
target_link_libraries(regex ${Boost_REGEX_LIBRARY})
target_link_libraries(regex ${JsonCpp})
