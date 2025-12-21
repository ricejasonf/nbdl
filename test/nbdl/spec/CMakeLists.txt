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

set(TEST_NBDL_SPEC_SOURCE_LIST
  context.cpp
  match.cpp
  match_cond.cpp
  visit.cpp
)

nbdl_catch_test_suite(build_targets "test.nbdl.spec"
  ${TEST_NBDL_SPEC_SOURCE_LIST}
)


foreach(SOURCE_FILE IN LISTS TEST_NBDL_SPEC_SOURCE_LIST)
  set_source_files_properties(
    ${SOURCE_FILE}
    OBJECT_DEPENDS
      ${nbdl_project_SOURCE_DIR}/include/nbdl/spec.sld)
endforeach()
