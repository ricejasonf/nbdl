#
# Copyright Jason Rice 2015
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

nbdl_catch_test_suite(build_targets "test.nbdl.core"
  context_match.cpp
  context.cpp
  context_state_consumer.cpp
  delta.cpp
  echo_provider.cpp
  entity.cpp
  map_store.cpp
  match_path.cpp
  message_api.cpp
  path.cpp
  pipe.cpp
  variant.cpp
)

# compile-time tests
nbdl_add_test("test.nbdl.core.detail.concept_pred"
  detail/concept_pred.cpp
)

