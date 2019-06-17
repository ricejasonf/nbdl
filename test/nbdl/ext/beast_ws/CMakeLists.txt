#
# Copyright Jason Rice 2019
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

nbdl_catch_test_suite(build_targets test.nbdl.ext.beast_ws
  accept.cpp
)

foreach(_target IN LISTS build_targets)
  target_link_libraries(${_target}
    ${CMAKE_THREAD_LIBS_INIT}
  )
endforeach()
