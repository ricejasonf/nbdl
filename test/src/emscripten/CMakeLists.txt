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

#note that --bind is being passed to the linker... weird
#target_link_libraries(test_emscripen nbdl --bind)

#bleh
target_include_directories(test_emscripten
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../../src"
  )
