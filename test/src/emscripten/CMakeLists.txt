#
# Copyright Jason Rice 2015
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

cmake_minimum_required(VERSION 2.8)

if (NOT EMSCRIPTEN)
  return()
endif (NOT EMSCRIPTEN)

#todo actually make tests for emscripten binders
return()

add_executable(test_emscripten
	main.cpp
	)

#bleh
target_include_directories(test_json
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../../src"
  )
