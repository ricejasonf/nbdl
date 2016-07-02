//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <assets/TestContext.hpp>
#include <nbdl/entity_members.hpp>
#include <nbdl/make_context.hpp>
#include <nbdl/map_store.hpp>

#include <catch.hpp>

// All of the tests will use messages
// for test_context::path_type<1> root2/my_entity1
nbdl::map_store<test_context::path<1>> make_test_store()
{ return {}; }

constexpr auto def = test_context_def::make(
  test_context::provider_tag{},
  test_context::provider_tag{},
  test_context::consumer_tag{},
  test_context::consumer_tag{}
);
namespace {
  auto context = nbdl::make_unique_context(def);
  auto push_api = context->cell<0>().push_api;
}

template <typename Variant, typename T>
bool test_variant_equal(Variant const& v, T const& t)
{
  return v.match(
    [&](T const& t_) { return hana::equal(t, t_); },
    [](auto&&)      { return false; }
  );
}

TEST_CASE("Apply action upstream create.", "[map_store][apply_action]")
{
  auto store = make_test_store();
  bool did_state_change = nbdl::apply_action(store,
    push_api.make_upstream_create_message(
      test_context::path<1>(1, 2),
      test_context::entity::my_entity<1>{2, 1}
    )
  );
  CHECK(did_state_change == false);
  CHECK(store.map.size() == 0);
}

TEST_CASE("Apply action downstream create.", "[map_store][apply_action]")
{
  auto store = make_test_store();
  bool did_state_change = nbdl::apply_action(store,
    push_api.make_downstream_create_message(
      test_context::path<1>(1, 2),
      test_context::entity::my_entity<1>{2, 1}
    )
  );
  CHECK(did_state_change == true);
  CHECK(store.map.size() == 1);
  CHECK(test_variant_equal(
    store.map[test_context::path<1>(1, 2)],
    test_context::entity::my_entity<1>{2, 1}
  ));
}

TEST_CASE("Apply action upstream read.", "[map_store][apply_action]")
{
  auto store = make_test_store();
  bool did_state_change = nbdl::apply_action(store,
    push_api.make_upstream_read_message(test_context::path<1>(1, 2))
  );
  CHECK(did_state_change == true);
  CHECK(store.map.size() == 1);
  auto node = store.map.find(test_context::path<1>(1, 2));
  REQUIRE(node != store.map.end());
  bool result = node->second.match(
    [](nbdl::unresolved)  { return true;  },
    [](auto&&)            { return false; }
  );
  CHECK(result);
}

TEST_CASE("Apply action downstream read with no value in store.", "[map_store][apply_action]")
{
  auto store = make_test_store();
  bool did_state_change = nbdl::apply_action(store,
    push_api.make_downstream_read_message(
      test_context::path<1>(1, 2),
      test_context::entity::my_entity<1>{2, 1}
    )
  );
  CHECK(did_state_change == true);
  CHECK(store.map.size() == 1);
  CHECK(test_variant_equal(
    store.map[test_context::path<1>(1, 2)],
    test_context::entity::my_entity<1>{2, 1}
  ));
}

TEST_CASE("Apply action downstream read with unresolved value in store.", "[map_store][apply_action]")
{
  auto store = make_test_store();
  store.map[test_context::path<1>(1, 2)] = nbdl::unresolved{};
  bool did_state_change = nbdl::apply_action(store,
    push_api.make_downstream_read_message(
      test_context::path<1>(1, 2),
      test_context::entity::my_entity<1>{2, 1}
    )
  );
  CHECK(did_state_change == true);
  CHECK(store.map.size() == 1);
  CHECK(test_variant_equal(
    store.map[test_context::path<1>(1, 2)],
    test_context::entity::my_entity<1>{2, 1}
  ));
}

TEST_CASE("Apply action downstream read with resolved value in store.", "[map_store][apply_action]")
{
  auto store = make_test_store();
  store.map[test_context::path<1>(1, 2)] = test_context::entity::my_entity<1>{2, 1};
  bool did_state_change = nbdl::apply_action(store,
    push_api.make_downstream_read_message(
      test_context::path<1>(1, 2),
      test_context::entity::my_entity<1>{0, 0}
    )
  );
  CHECK(did_state_change == false);
  CHECK(store.map.size() == 1);
  CHECK(test_variant_equal(
    store.map[test_context::path<1>(1, 2)],
    test_context::entity::my_entity<1>{2, 1}
  ));
}

TEST_CASE("Apply action downstream read with not_found value in store.", "[map_store][apply_action]")
{
  auto store = make_test_store();
  store.map[test_context::path<1>(1, 2)] = nbdl::not_found{};
  bool did_state_change = nbdl::apply_action(store,
    push_api.make_downstream_read_message(
      test_context::path<1>(1, 2),
      test_context::entity::my_entity<1>{0, 0}
    )
  );
  CHECK(did_state_change == false);
  CHECK(store.map.size() == 1);
  bool result = store.map[test_context::path<1>(1, 2)].match(
    [](nbdl::not_found) { return true;  },
    [](auto&&)          { return false; }
  );
}

TEST_CASE("Apply action upstream update raw.", "[map_store][apply_action]")
{
  auto store = make_test_store();
  bool did_state_change = nbdl::apply_action(store,
    push_api.make_upstream_update_raw_message(
      test_context::path<1>(1, 2),
      test_context::entity::my_entity<1>{2, 1}
    )
  );
  CHECK(did_state_change == false);
  CHECK(store.map.size() == 0);
}

TEST_CASE("Apply action downstream update raw with no value.", "[map_store][apply_action]")
{
  // update_raw always overwrites
  auto store = make_test_store();
  bool did_state_change = nbdl::apply_action(store,
    push_api.make_downstream_update_raw_message(
      test_context::path<1>(1, 2),
      test_context::entity::my_entity<1>{2, 1}
    )
  );
  CHECK(did_state_change == true);
  CHECK(store.map.size() == 1);
  CHECK(test_variant_equal(
    store.map[test_context::path<1>(1, 2)],
    test_context::entity::my_entity<1>{2, 1}
  ));
}

TEST_CASE("Apply action downstream update raw with value.", "[map_store][apply_action]")
{
  // update_raw always overwrites
  auto store = make_test_store();
  store.map[test_context::path<1>(1, 2)] = test_context::entity::my_entity<1>{0, 0};
  bool did_state_change = nbdl::apply_action(store,
    push_api.make_downstream_update_raw_message(
      test_context::path<1>(1, 2),
      test_context::entity::my_entity<1>{2, 1}
    )
  );
  CHECK(did_state_change == true);
  CHECK(store.map.size() == 1);
  CHECK(test_variant_equal(
    store.map[test_context::path<1>(1, 2)],
    test_context::entity::my_entity<1>{2, 1}
  ));
}

TEST_CASE("Apply action upstream delete.", "[map_store][apply_action]")
{
  auto store = make_test_store();
  bool did_state_change = nbdl::apply_action(store,
    push_api.make_upstream_delete_message(
      test_context::path<1>(1, 2)
    )
  );
  CHECK(did_state_change == false);
  CHECK(store.map.size() == 0);
}

TEST_CASE("Apply action downstream delete.", "[map_store][apply_action]")
{
  auto store = make_test_store();
  store.map[test_context::path<1>(1, 2)] = test_context::entity::my_entity<1>{0, 0};
  bool did_state_change = nbdl::apply_action(store,
    push_api.make_downstream_delete_message(
      test_context::path<1>(1, 2)
    )
  );
  CHECK(did_state_change == true);
  CHECK(store.map.size() == 0);
}

TEST_CASE("Apply action downstream delete with value present.", "[map_store][apply_action]")
{
  auto store = make_test_store();
  store.map[test_context::path<1>(1, 2)] = test_context::entity::my_entity<1>{0, 0};
  bool did_state_change = nbdl::apply_action(store,
    push_api.make_downstream_delete_message(
      test_context::path<1>(1, 2)
    )
  );
  CHECK(did_state_change == true);
  CHECK(store.map.size() == 0);
}

#if 0
// TODO: use nbdl::apply_foreign_action
TEST_CASE("Apply action with foreign path.", "[map_store][apply_action]")
{
  auto store = make_test_store();
  bool did_state_change = nbdl::apply_action(store,
    push_api.make_downstream_create_message(
      test_context::path<2>(0, 0),
      test_context::entity::my_entity<2>{0, 0}
    )
  );
  CHECK(did_state_change == false);
  CHECK(store.map.size() == 0);
}
#endif
