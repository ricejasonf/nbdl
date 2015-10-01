cmake_minimum_required(VERSION 2.8)

project(nbdl)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

add_definitions(-std=c++11)

if (${EMSCRIPTEN})
	add_library(nbdl STATIC 
		Regex.cpp
		)
else (${EMSCRIPTEN})
	add_library(nbdl STATIC 
		binders/jsoncpp/Read.cpp
		Regex.cpp
		openssl/Base64Encode.cpp
		servers/ws/HttpRequestParser.cpp
		servers/ws/HttpResponse.cpp
    servers/ws/MessageParser.cpp
    servers/ws/MessageGenerator.cpp
		)
endif (${EMSCRIPTEN})

include(../nbdl.cmake)

if (${EMSCRIPTEN})
else (${EMSCRIPTEN})
  #target_link_libraries(nbdl PRIVATE OPEN_SSL_LIBRARIES)
endif (${EMSCRIPTEN})

