cmake_minimum_required(VERSION 2.8)

project(print)

add_definitions(-std=c++11)
if (${EMSCRIPTEN})
	add_definitions(--bind)
endif (${EMSCRIPTEN})

add_executable(print
	print.cpp
	)

SET(LIBNBDL_DIR
	/home/jason/Projects/NewHotness
	)
include_directories(${LIBNBDL_DIR}/src)
include(${LIBNBDL_DIR}/nbdl.cmake)
target_link_libraries(print ${NBDL_LIB_DIR}/libnbdl.a --bind)
if (${EMSCRIPTEN})
else (${EMSCRIPTEN})
	target_link_libraries(print ${Boost_REGEX_LIBRARY})
	target_link_libraries(print ${JsonCpp})
endif (${EMSCRIPTEN})
