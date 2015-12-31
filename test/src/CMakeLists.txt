#
# Copyright Jason Rice 2015
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

cmake_minimum_required(VERSION 2.8)

if (EMSCRIPTEN)
  add_subdirectory(emscripten)
  return()
endif (EMSCRIPTEN)

add_subdirectory(websocket)
add_subdirectory(json)

nbdl_catch_test_suite(build_targets "test.nbdl.core"
  factorial.cpp
  path.cpp
  variant.cpp
  store.cpp
  context_read.cpp
)

#requires boost right now
#msg_id.cpp 
