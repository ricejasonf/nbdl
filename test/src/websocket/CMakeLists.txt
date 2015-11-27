#
# Copyright Jason Rice 2015
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

cmake_minimum_required(VERSION 2.8)

if (NOT OPENSSL_FOUND)
  message(WARNING "Skipping test_websocket.")
  return()
endif (NOT OPENSSL_FOUND)

add_executable(test_websocket
	main.cpp
  generate_handshake_response.cpp
  messages.cpp
  parse_handshake_request.cpp
	)

#bleh
target_include_directories(test_websocket
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../../src"
  )

target_link_libraries(test_websocket nbdl_servers_ws)
target_link_libraries(test_websocket ${OPENSSL_LIBRARIES})
