cmake_minimum_required(VERSION 2.8)

project(test)

add_definitions(-std=c++11)
add_definitions(-Wall)
add_definitions(-Wextra)
if (${EMSCRIPTEN})
	add_definitions(--bind)
endif (${EMSCRIPTEN})

add_executable(test
	test.cpp
	factorial.cpp
	bind_json.cpp
	path.cpp
	#msg_id.cpp
	#api_read.cpp
	variant.cpp
	variant_callback.cpp
	store.cpp
	api_definition.cpp
	context_read.cpp
	)

SET(LIBNBDL_DIR
	/home/jason/Projects/NewHotness
	)
include_directories(${LIBNBDL_DIR}/src)
include(${LIBNBDL_DIR}/nbdl.cmake)
if (${EMSCRIPTEN})
	target_link_libraries(test ${NBDL_LIB_DIR}/libnbdl.a --bind)
else (${EMSCRIPTEN})
	target_link_libraries(test ${NBDL_LIB_DIR}/libnbdl.a)
	target_link_libraries(test ${Boost_REGEX_LIBRARY})
	target_link_libraries(test ${JsonCpp})
endif (${EMSCRIPTEN})
