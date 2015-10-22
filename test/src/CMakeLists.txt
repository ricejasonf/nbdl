#
# Copyright Jason Rice 2015
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

cmake_minimum_required(VERSION 2.8)

add_executable(test_
	test.cpp
	factorial.cpp
	bind_json.cpp
	path.cpp
	#msg_id.cpp #uses boost::uuid
	variant.cpp
	variant_callback.cpp
	store.cpp
	api_definition.cpp
	context_read.cpp
	websocket/parse_handshake_request.cpp
	websocket/generate_handshake_response.cpp
  websocket/messages.cpp
	)

#bleh
target_include_directories(test_ 
  PUBLIC ${BOOST_HANA_INCLUDE_DIR}
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../src"
  )

#find externals deps
if (${EMSCRIPTEN})
else (${EMSCRIPTEN})
  #find_package(OpenSSL REQUIRED)
  #set(Boost_USE_STATIC_LIBS OFF)
  #set(Boost_USE_MULTITHREADED OFF)
  #set(Boost_USE_STATIC_RUNTIME OFF)
  #find_package(Boost COMPONENTS regex REQUIRED)
  #find_library(JsonCpp jsoncpp)
  target_include_directories(test_
    PUBLIC ${OPENSSL_INCLUDE_DIR}
    #PUBLIC ${Boost_INCLUDE_DIR}
      )
endif(${EMSCRIPTEN})

if (${EMSCRIPTEN})
  target_link_libraries(test_ nbdl --bind)
else (${EMSCRIPTEN})
  target_link_libraries(test_ nbdl)
  target_link_libraries(test_ ${JsonCpp})
  target_link_libraries(test_ ${OPENSSL_LIBRARIES})
endif (${EMSCRIPTEN})
