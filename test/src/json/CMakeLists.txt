#
# Copyright Jason Rice 2015
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

cmake_minimum_required(VERSION 2.8)

if (NOT JsonCpp)
  message(WARNING "Skipping test_json.")
  return()
endif (NOT JsonCpp)

add_executable(test_json
	main.cpp
  bind_json.cpp
  bind_json_variant.cpp
  bind_json_sequence.cpp
	)

#bleh
target_include_directories(test_json
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../../src"
  )

target_link_libraries(test_json nbdl_binders_jsoncpp)
#target_link_libraries(test_json ${JsonCpp})
