#
# Copyright Jason Rice 2016
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

include_directories("../../../test/include")

nbdl_catch_test_suite(build_targets "example.test.tiles"
  context.cpp
  game_apply_move.cpp
)
