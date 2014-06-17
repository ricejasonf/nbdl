#include this at the end of your applications CMakeLists.txt file

get_filename_component(ARCUS_TARGET_PLATFORM
	${CMAKE_BINARY_DIR}
	NAME
	)
message(STATUS
	"ARCUS_TARGET_PLATFORM is: ${ARCUS_TARGET_PLATFORM}"
	)
set(ARCUS_LIB_DIR ${LIBARCUS_DIR}/build/${ARCUS_TARGET_PLATFORM})

if (${ARCUS_APP_BIN_NAME})
	message(STATUS "ARCUS_APP_BIN_NAME: ${ARCUS_APP_BIN_NAME}")
else (${ARCUS_APP_BIN_NAME})
	set(ARCUS_APP_BIN_NAME ${PROJECT_NAME})
	message(STATUS "Implying ARCUS_APP_BIN_NAME: ${ARCUS_APP_BIN_NAME}")
endif (${ARCUS_APP_BIN_NAME})
#regex stuff
if (${EMSCRIPTEN})
	if (NOT ${PROJECT_NAME} STREQUAL "arcus")
		#run em++ to compile bitcode into javascript
		add_custom_target("${PROJECT_NAME}.js"
			ALL
			cp ${ARCUS_APP_BIN_NAME} ${ARCUS_APP_BIN_NAME}.bc
				&& em++ ${ARCUS_APP_BIN_NAME}.bc -o ${PROJECT_NAME}.js
			COMMENT "Building Javascript with emscripten..."
			)
	endif (NOT ${PROJECT_NAME} STREQUAL "arcus")
else (${EMSCRIPTEN})
	set(Boost_USE_STATIC_LIBS ON)
	set(Boost_USE_MULTITHREADED ON)
	set(Boost_USE_STATIC_RUNTIME OFF)
	find_package(Boost COMPONENTS regex REQUIRED)
	include_directories(${Boost_INCLUDE_DIR})
	find_library(JsonCpp jsoncpp)
endif (${EMSCRIPTEN})
