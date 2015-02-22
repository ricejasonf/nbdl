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
		#run em++ to compile bitcode into javascript
		add_custom_target("${PROJECT_NAME}.js"
			ALL
			cp ${NBDL_APP_BIN_NAME} ${NBDL_APP_BIN_NAME}.bc
				&& em++ ${NBDL_APP_BIN_NAME}.bc -o ${PROJECT_NAME}.js
			COMMENT "Building Javascript with emscripten..."
			)
	endif (NOT ${PROJECT_NAME} STREQUAL "nbdl")
else (${EMSCRIPTEN})
	set(Boost_USE_STATIC_LIBS ON)
	set(Boost_USE_MULTITHREADED ON)
	set(Boost_USE_STATIC_RUNTIME OFF)
	find_package(Boost COMPONENTS regex REQUIRED)
	include_directories(${Boost_INCLUDE_DIR})
	find_library(JsonCpp jsoncpp)
endif (${EMSCRIPTEN})
