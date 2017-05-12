#
# Copyright Jason Rice 2015
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

nbdl_catch_test_suite(build_targets "test.nbdl.core"
  apply_action.cpp
  context.cpp
  context_match.cpp
  context_state_consumer.cpp
  context_store.cpp
  delta.cpp
  detail/string_concat.cpp
  echo_producer.cpp
  entity.cpp
  map_store.cpp
  match_path.cpp
  message_api.cpp
  params_promise.cpp
  path_promise.cpp
  pipe.cpp
  variant.cpp
)

# compile-time tests
nbdl_add_test("test.nbdl.core.detail.concept_pred"
  detail/concept_pred.cpp
)
nbdl_add_test("test.nbdl.core.ui_spec"
  ui_spec.cpp
)
