#
# Copyright Jason Rice 2025
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

#nbdl_catch_test_suite(build_targets "test.nbdl.spec"
#  basic.cpp
#)

# compile-time tests

nbdl_add_test("test.nbdl.spec.basic"
  basic.cpp
)
nbdl_catch_test_suite(build_targets "test.nbdl.spec"
  context.cpp
  match.cpp
)
