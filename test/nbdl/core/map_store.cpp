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

namespace test_map_store
{
  struct my_context { };

  nbdl::map_store<test_context::path<1>, test_context::entity::my_entity<1>> make_test_store()
  { return {}; }
}

namespace nbdl
{
  template <>
  struct make_def_impl<test_map_store::my_context>
  {
    static constexpr auto apply()
    {
      return test_context_def::make(
        test_context::producer_tag{},
        test_context::producer_tag{},
        test_context::consumer_tag{},
        test_context::consumer_tag{},
        nbdl::map_store_tag{}
      );
    }
  };
}

template <typename Variant, typename T>
bool test_variant_equal(Variant const& v, T const& t)
{
  bool result = false;
  v.match(
    [&](T const& t_)  { result = hana::equal(t, t_); },
    [](auto&&)        { }
  );
  return result;
}

TEST_CASE("Apply action upstream create.", "[map_store][apply_action]")
{
  auto context = nbdl::make_unique_context<test_map_store::my_context>();
  auto push_api = context->cell<0>().push_api;
  using Path = test_context::path<1>;
  using CreatePath = typename nbdl::detail::make_create_path<Path>::type;

  auto store = test_map_store::make_test_store();
  bool did_state_change = nbdl::apply_action(store,
    push_api.message_api().make_upstream_create_message(
      CreatePath(1, hana::type_c<test_context::key<test_context::entity::my_entity<1>>>),
      test_context::entity::my_entity<1>{2, 1}
    )
  );
  CHECK(did_state_change == false);
  CHECK(store.map.size() == 0);
}

TEST_CASE("Apply action downstream create.", "[map_store][apply_action]")
{
  auto context = nbdl::make_unique_context<test_map_store::my_context>();
  auto push_api = context->cell<0>().push_api;

  auto store = test_map_store::make_test_store();
  bool did_state_change = nbdl::apply_action(store,
    push_api.message_api().make_downstream_create_message(
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
  auto context = nbdl::make_unique_context<test_map_store::my_context>();
  auto push_api = context->cell<0>().push_api;

  auto store = test_map_store::make_test_store();
  bool did_state_change = nbdl::apply_action(store,
    push_api.message_api().make_upstream_read_message(test_context::path<1>(1, 2))
  );
  // A placeholder (nbdl::unresolved) is created, but
  // technically the state does not change.
  CHECK(did_state_change == false);
  CHECK(store.map.size() == 1);
  auto node = store.map.find(test_context::path<1>(1, 2));
  REQUIRE(node != store.map.end());
  bool result = false;
  node->second.match(
    [&](nbdl::unresolved)  { result = true;  },
    [](auto&&)            { }
  );
  CHECK(result);
}

TEST_CASE("Apply action downstream read with no value in store.", "[map_store][apply_action]")
{
  auto context = nbdl::make_unique_context<test_map_store::my_context>();
  auto push_api = context->cell<0>().push_api;

  auto store = test_map_store::make_test_store();
  bool did_state_change = nbdl::apply_action(store,
    push_api.message_api().make_downstream_read_message(
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
  auto context = nbdl::make_unique_context<test_map_store::my_context>();
  auto push_api = context->cell<0>().push_api;

  auto store = test_map_store::make_test_store();
  store.map[test_context::path<1>(1, 2)] = nbdl::unresolved{};
  bool did_state_change = nbdl::apply_action(store,
    push_api.message_api().make_downstream_read_message(
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
  auto context = nbdl::make_unique_context<test_map_store::my_context>();
  auto push_api = context->cell<0>().push_api;

  auto store = test_map_store::make_test_store();
  store.map[test_context::path<1>(1, 2)] = test_context::entity::my_entity<1>{2, 1};
  bool did_state_change = nbdl::apply_action(store,
    push_api.message_api().make_downstream_read_message(
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
  auto context = nbdl::make_unique_context<test_map_store::my_context>();
  auto push_api = context->cell<0>().push_api;

  auto store = test_map_store::make_test_store();
  store.map[test_context::path<1>(1, 2)] = nbdl::not_found{};
  bool did_state_change = nbdl::apply_action(store,
    push_api.message_api().make_downstream_read_message(
      test_context::path<1>(1, 2),
      test_context::entity::my_entity<1>{0, 0}
    )
  );
  CHECK(did_state_change == false);
  CHECK(store.map.size() == 1);
  bool result = false;
  store.map[test_context::path<1>(1, 2)].match(
    [&](nbdl::not_found) { result = true;  },
    [](auto&&)           { }
  );
  CHECK(result);
}

TEST_CASE("Apply action upstream update.", "[map_store][apply_action]")
{
  auto context = nbdl::make_unique_context<test_map_store::my_context>();
  auto push_api = context->cell<0>().push_api;

  auto store = test_map_store::make_test_store();
  bool did_state_change = nbdl::apply_action(store,
    push_api.message_api().make_upstream_update_message(
      test_context::path<1>(1, 2),
      test_context::entity::my_entity<1>{2, 1}
    )
  );
  CHECK(did_state_change == false);
  CHECK(store.map.size() == 0);
}

TEST_CASE("Apply action downstream update with no value.", "[map_store][apply_action]")
{
  auto context = nbdl::make_unique_context<test_map_store::my_context>();
  auto push_api = context->cell<0>().push_api;

  // update always overwrites
  auto store = test_map_store::make_test_store();
  bool did_state_change = nbdl::apply_action(store,
    push_api.message_api().make_downstream_update_message(
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

TEST_CASE("Apply action downstream update with value.", "[map_store][apply_action]")
{
  auto context = nbdl::make_unique_context<test_map_store::my_context>();
  auto push_api = context->cell<0>().push_api;

  // update always overwrites
  auto store = test_map_store::make_test_store();
  store.map[test_context::path<1>(1, 2)] = test_context::entity::my_entity<1>{0, 0};
  bool did_state_change = nbdl::apply_action(store,
    push_api.message_api().make_downstream_update_message(
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
  auto context = nbdl::make_unique_context<test_map_store::my_context>();
  auto push_api = context->cell<0>().push_api;

  auto store = test_map_store::make_test_store();
  bool did_state_change = nbdl::apply_action(store,
    push_api.message_api().make_upstream_delete_message(
      test_context::path<1>(1, 2)
    )
  );
  CHECK(did_state_change == false);
  CHECK(store.map.size() == 0);
}

TEST_CASE("Apply action downstream delete.", "[map_store][apply_action]")
{
  auto context = nbdl::make_unique_context<test_map_store::my_context>();
  auto push_api = context->cell<0>().push_api;

  auto store = test_map_store::make_test_store();
  store.map[test_context::path<1>(1, 2)] = test_context::entity::my_entity<1>{0, 0};
  bool did_state_change = nbdl::apply_action(store,
    push_api.message_api().make_downstream_delete_message(
      test_context::path<1>(1, 2)
    )
  );
  CHECK(did_state_change == true);
  CHECK(store.map.size() == 0);
}

TEST_CASE("Apply action downstream delete with value present.", "[map_store][apply_action]")
{
  auto context = nbdl::make_unique_context<test_map_store::my_context>();
  auto push_api = context->cell<0>().push_api;

  auto store = test_map_store::make_test_store();
  store.map[test_context::path<1>(1, 2)] = test_context::entity::my_entity<1>{0, 0};
  bool did_state_change = nbdl::apply_action(store,
    push_api.message_api().make_downstream_delete_message(
      test_context::path<1>(1, 2)
    )
  );
  CHECK(did_state_change == true);
  CHECK(store.map.size() == 0);
}

TEST_CASE("Apply action with foreign path.", "[map_store][apply_action]")
{
  auto context = nbdl::make_unique_context<test_map_store::my_context>();
  auto push_api = context->cell<0>().push_api;

  auto store = test_map_store::make_test_store();
  bool did_state_change = false;
  nbdl::apply_foreign_action(store,
    push_api.message_api().make_downstream_create_message(
      test_context::path<2>(0, 0),
      test_context::entity::my_entity<2>{0, 0}
    ),
    [&](auto const&)
    {
      did_state_change = true;
    }
  );
  CHECK(did_state_change == false);
  CHECK(store.map.size() == 0);
}
