#
# Copyright Jason Rice 2017
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

if (NOT NBDL_HAS_UIKIT)
  return()
endif (NOT NBDL_HAS_UIKIT)

#nbdl_catch_test_suite(build_targets "test.nbdl.ui_view"
#  renderer.mm
#  detail/manips.mm
#)

nbdl_add_test("test.nbdl.ui_view.detail.flatten_spec"
  detail/flatten_spec.mm
)
