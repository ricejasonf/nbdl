#
# Copyright Jason Rice 2015
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

if (NOT JsonCpp)
  message(WARNING "Skipping test.nbdl.binders.jsoncpp.")
  return()
endif (NOT JsonCpp)

nbdl_catch_test_suite(build_targets test.nbdl.binders.jsoncpp
  bind.cpp
  bind_variant.cpp
  bind_sequence.cpp
)

foreach(_target IN LISTS build_targets)
  target_link_libraries(${_target} ${JsonCpp})
endforeach()
