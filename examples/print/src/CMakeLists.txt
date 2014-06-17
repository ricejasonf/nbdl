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

#get_filename_component(TARGET_PLATFORM
#	${CMAKE_BINARY_DIR}
#	NAME
#	)
#message(STATUS
#	"TARGET_PLATFORM is: ${TARGET_PLATFORM}"
#	)
#SET(LIBARCUS_BIN_DIR
#	${LIBARCUS_DIR}/build/${TARGET_PLATFORM}/
#	)
#include_directories(${LIBARCUS_DIR}/src)
#link_directories(${LIBARCUS_BIN_DIR})
#
#set(EXEC_NAME "print")
#
#target_link_libraries(${EXEC_NAME} arcus)
#
#if (${TARGET_PLATFORM} STREQUAL "emscripten")
#	#em++ is finicky about input file extensions 
#	set(EXEC_NAME ${EXEC_NAME})
#	#run em++ to compile bitcode into javascript
#	add_custom_target("${PROJECT_NAME}.js"
#		ALL
#		cp ${PROJECT_NAME} ${PROJECT_NAME}.bc && $em++ ${EXEC_NAME} -o ${PROJECT_NAME}.js
#		COMMENT "Building Javascript with emscripten..."
#		)
#else (${TARGET_PLATFORM} STREQUAL "emscripten")
#	set(Boost_USE_STATIC_LIBS OFF)
#	set(Boost_USE_MULTITHREADED ON)
#	set(Boost_USE_STATIC_RUNTIME OFF)
#	find_package(Boost COMPONENTS regex REQUIRED)
#	include_directories(${Boost_INCLUDE_DIR})
#	target_link_libraries(${EXEC_NAME} ${Boost_REGEX_LIBRARY})
#endif (${TARGET_PLATFORM} STREQUAL "emscripten")
