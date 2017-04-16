#
# Copyright Jason Rice 2016
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

nbdl_catch_dom_test_suite(build_targets "test.nbdl.webui"
  renderer.cpp
  detail/dom_manips.cpp
)

nbdl_add_test("test.nbdl.webui.detail.flatten_spec"
  detail/flatten_spec.cpp
)
