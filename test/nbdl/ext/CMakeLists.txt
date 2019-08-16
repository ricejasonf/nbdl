#
# Copyright Jason Rice 2018
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

nbdl_catch_test_suite(build_targets "test.nbdl.ext"
  std/unordered_map.cpp
)

if (NOT EMSCRIPTEN)
add_subdirectory(beast_ws)
endif()
