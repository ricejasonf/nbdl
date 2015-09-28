#include this at the end of your applications CMakeLists.txt file

get_filename_component(NBDL_TARGET_PLATFORM
	${CMAKE_BINARY_DIR}
	NAME
	)
message(STATUS
	"NBDL_TARGET_PLATFORM is: ${NBDL_TARGET_PLATFORM}"
	)
set(NBDL_LIB_DIR ${LIBNBDL_DIR}/build/${NBDL_TARGET_PLATFORM})

if (${NBDL_APP_BIN_NAME})
	message(STATUS "NBDL_APP_BIN_NAME: ${NBDL_APP_BIN_NAME}")
else (${NBDL_APP_BIN_NAME})
	set(NBDL_APP_BIN_NAME ${PROJECT_NAME})
	message(STATUS "Implying NBDL_APP_BIN_NAME: ${NBDL_APP_BIN_NAME}")
endif (${NBDL_APP_BIN_NAME})
#regex stuff
if (${EMSCRIPTEN})
	if (NOT ${PROJECT_NAME} STREQUAL "nbdl")
# below is deprecated use 
# emcmake cmake -DCMAKE_BUILD_TYPE=blah ../../src/
# this runs em++ to compile bitcode into javascript (obsoleted by emcmake)
#		add_custom_target("${PROJECT_NAME}.js"
#			ALL
#			cp ${NBDL_APP_BIN_NAME} ${NBDL_APP_BIN_NAME}.bc
#				&& em++ --bind ${NBDL_APP_BIN_NAME}.bc -o ${PROJECT_NAME}.js
#			COMMENT "Building Javascript with emscripten..."
#			)
	endif (NOT ${PROJECT_NAME} STREQUAL "nbdl")
else (${EMSCRIPTEN})
	find_package(OpenSSL REQUIRED)
	include_directories(${OPENSSL_INCLUDE_DIR})
	set(Boost_USE_STATIC_LIBS OFF)
	set(Boost_USE_MULTITHREADED OFF)
	set(Boost_USE_STATIC_RUNTIME OFF)
	find_package(Boost COMPONENTS regex REQUIRED)
	include_directories(${Boost_INCLUDE_DIR})
	find_library(JsonCpp jsoncpp)
endif (${EMSCRIPTEN})
