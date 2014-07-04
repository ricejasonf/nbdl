cmake_minimum_required(VERSION 2.8)

project(chat)

add_definitions(-std=c++11)
set(CMAKE_BUILD_TYPE Debug)

add_executable(chat
	chat.cpp
	)

add_subdirectory(tests)

SET(LIBARCUS_DIR
	/home/jason/Projects/NewHotness
	)
include_directories(${LIBARCUS_DIR}/src)
include(${LIBARCUS_DIR}/arcus.cmake)
target_link_libraries(chat ${ARCUS_LIB_DIR}/libarcus.a)
target_link_libraries(chat ${Boost_REGEX_LIBRARY})
target_link_libraries(chat ${JsonCpp})
