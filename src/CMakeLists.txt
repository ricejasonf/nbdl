#
# Copyright Jason Rice 2015
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

cmake_minimum_required(VERSION 2.8)

#todo break up components

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

#find_package(Hana REQUIRED)

if (${EMSCRIPTEN})

else (${EMSCRIPTEN})
  #find_package(OpenSSL REQUIRED)
  #set(Boost_USE_STATIC_LIBS OFF)
  #set(Boost_USE_MULTITHREADED OFF)
  #set(Boost_USE_STATIC_RUNTIME OFF)
  #find_package(Boost COMPONENTS regex REQUIRED)
  #find_library(JsonCpp jsoncpp)

  target_include_directories(nbdl
    PUBLIC ${OPENSSL_INCLUDE_DIR}
    PUBLIC ${BOOST_HANA_INCLUDE_DIR}
    #PUBLIC ${Boost_INCLUDE_DIR}
    )

  target_link_libraries(nbdl ${JsonCpp})
  target_link_libraries(nbdl ${OPENSSL_LIBRARIES})
endif (NOT ${EMSCRIPTEN})
