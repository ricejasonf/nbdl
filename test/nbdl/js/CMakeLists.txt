#
# Copyright Jason Rice 2017
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

nbdl_catch_test_suite(build_targets "test.nbdl.js"
  callback.cpp
  get_element_by_id.cpp
  promise.cpp
  val.cpp
)

em_link_js_library(test.nbdl.js "c_library.js")
em_link_js_library(individual.test.nbdl.js.promise "c_library.js")
