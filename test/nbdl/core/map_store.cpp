//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/map_store.hpp>

#include <boost/hana/tuple.hpp>
#include <catch.hpp>

namespace hana = boost::hana;
namespace message = nbdl::message;

namespace
{
  template <int i> struct PathKey { int value; };
  template <int i> struct Payload { int value; };
  struct FakeUid { int value; };

  using Path = hana::tuple<PathKey<1>, PathKey<2>>;
}

namespace nbdl
{
  template <int i>
  struct bind_sequence_impl<PathKey<i>>
  {
    template <typename BindFn>
    static constexpr auto apply(PathKey<i> p, BindFn&& f)
    {
      return std::forward<BindFn>(f)(p.value);
    }
  };
}

namespace boost::hana
{
  template <int i>
  struct equal_impl<PathKey<i>, PathKey<i>>
  {
    template <typename T1, typename T2>
    static constexpr auto apply(T1 const& t1, T2 const& t2)
    { return t1.value == t2.value; }
  };

  template <int i>
  struct equal_impl<Payload<i>, Payload<i>>
  {
    template <typename T1, typename T2>
    static constexpr auto apply(T1 const& t1, T2 const& t2)
    { return t1.value == t2.value; }
  };

  template <>
  struct equal_impl<FakeUid, FakeUid>
  {
    static constexpr auto apply(FakeUid const& t1, FakeUid const& t2)
    { return t1.value == t2.value; }
  };
}

namespace
{
  nbdl::map_store<Path, Payload<2>> make_test_store()
  { return {}; }
}

TEST_CASE("Apply message upstream create.", "[map_store][apply_message]")
{
  auto path = message::make_create_path<PathKey<2>>(hana::make_tuple(PathKey<1>{1}));
  auto uid = FakeUid{242};
  auto payload = Payload<2>{42};

  auto store = make_test_store();
  bool did_state_change = nbdl::apply_message(
    store
  , message::make_upstream_create(
      path
    , uid
    , payload
    )
  );
  CHECK(did_state_change == false);
  CHECK(store.map.size() == 0);
}

TEST_CASE("Apply message downstream create.", "[map_store][apply_message]")
{
  auto path = hana::make_tuple(PathKey<1>{11}, PathKey<2>{22});
  auto uid = FakeUid{242};
  auto payload = Payload<2>{42};

  auto store = make_test_store();

  bool did_state_change = nbdl::apply_message(
    store
  , message::make_downstream_create(
      hana::make_tuple(PathKey<1>{11}, PathKey<2>{22})
    , uid
    , Payload<2>{42}
    , true
    )
  );
  CHECK(did_state_change == true);
  CHECK(store.map.size() == 1);

  bool result = false;
  nbdl::match(store, path, hana::overload_linearly(
    [&](Payload<2> const& result_payload)
    {
      result = hana::equal(result_payload, payload);
    }
  , nbdl::noop
  ));

  CHECK(result);
}

TEST_CASE("Apply message upstream read.", "[map_store][apply_message]")
{
  auto path = hana::make_tuple(PathKey<1>{11}, PathKey<2>{22});
  auto uid = FakeUid{242};

  auto store = make_test_store();

  bool did_state_change = nbdl::apply_message(
    store
  , message::make_upstream_read(path, uid)
  );

  // A placeholder (nbdl::unresolved) is created, but
  // technically the state does not change.
  CHECK(not did_state_change);
  CHECK(store.map.size() == 1);

  bool result = false;
  nbdl::match(store, path, hana::overload_linearly(
    [&](nbdl::unresolved)
    {
      result = true;
    }
  , nbdl::noop
  ));

  CHECK(result);
}

TEST_CASE("Apply message downstream read with no value in store.", "[map_store][apply_message]")
{
  auto path = hana::make_tuple(PathKey<1>{11}, PathKey<2>{22});
  auto uid = FakeUid{242};
  auto payload = Payload<2>{42};

  auto store = make_test_store();

  bool did_state_change = nbdl::apply_message(
    store
  , message::make_downstream_read(
      path
    , uid
    , payload
    )
  );
  CHECK(did_state_change == true);
  CHECK(store.map.size() == 1);

  bool result = false;
  nbdl::match(store, path, hana::overload_linearly(
    [&](Payload<2> const& result_payload)
    {
      result = hana::equal(result_payload, payload);
    }
  , nbdl::noop
  ));

  CHECK(result);
}

TEST_CASE("Apply message downstream read with unresolved value in store.", "[map_store][apply_message]")
{
  auto path = hana::make_tuple(PathKey<1>{11}, PathKey<2>{22});
  auto uid = FakeUid{242};
  auto payload = Payload<2>{42};

  auto store = make_test_store();

  store.map[path] = nbdl::unresolved{}; // this is impl stuff

  bool did_state_change = nbdl::apply_message(
    store
  , message::make_downstream_read(
      path
    , uid
    , payload
    )
  );

  CHECK(did_state_change == true);
  CHECK(store.map.size() == 1);

  bool result = false;
  nbdl::match(store, path, hana::overload_linearly(
    [&](Payload<2> const& result_payload)
    {
      result = hana::equal(result_payload, payload);
    }
  , nbdl::noop
  ));

  CHECK(result);
}

TEST_CASE("Apply message downstream read with resolved value in store.", "[map_store][apply_message]")
{
  auto path = hana::make_tuple(PathKey<1>{11}, PathKey<2>{22});
  auto uid = FakeUid{242};
  auto payload = Payload<2>{42};
  auto original_payload = Payload<2>{223};

  auto store = make_test_store();

  store.map[path] = original_payload;

  bool did_state_change = nbdl::apply_message(
    store
  , message::make_downstream_read(
      path
    , uid
    , payload
    )
  );

  CHECK(did_state_change == false);
  CHECK(store.map.size() == 1);

  bool result = false;
  nbdl::match(store, path, hana::overload_linearly(
    [&](Payload<2> const& result_payload)
    {
      result = hana::equal(result_payload, original_payload);
    }
  , nbdl::noop
  ));

  CHECK(result);
}

TEST_CASE("Apply message downstream read with not_found value in store.", "[map_store][apply_message]")
{
  auto path = hana::make_tuple(PathKey<1>{11}, PathKey<2>{22});
  auto uid = FakeUid{242};
  auto payload = Payload<2>{42};
  auto original_payload = nbdl::not_found{};

  auto store = make_test_store();

  store.map[path] = original_payload;

  bool did_state_change = nbdl::apply_message(
    store
  , message::make_downstream_read(
      path
    , uid
    , payload
    )
  );

  CHECK(did_state_change == false);
  CHECK(store.map.size() == 1);

  bool result = false;
  nbdl::match(store, path, hana::overload_linearly(
    [&](nbdl::not_found)
    {
      result = true;
    }
  , nbdl::noop
  ));

  CHECK(result);
}

TEST_CASE("Apply message upstream update.", "[map_store][apply_message]")
{
  auto path = hana::make_tuple(PathKey<1>{11}, PathKey<2>{22});
  auto uid = FakeUid{242};
  auto payload = Payload<2>{42};

  auto store = make_test_store();

  bool did_state_change = nbdl::apply_message(
    store
  , message::make_upstream_update(
      path
    , uid
    , payload
    )
  );

  CHECK(did_state_change == false);
  CHECK(store.map.size() == 0);
}

TEST_CASE("Apply message downstream update with no value.", "[map_store][apply_message]")
{
  auto path = hana::make_tuple(PathKey<1>{11}, PathKey<2>{22});
  auto uid = FakeUid{242};
  auto payload = Payload<2>{42};

  auto store = make_test_store();

  // update always overwrites
  bool did_state_change = nbdl::apply_message(
    store
  , message::make_downstream_update(
      path
    , uid
    , payload
    , true
    )
  );

  CHECK(did_state_change == true);
  CHECK(store.map.size() == 1);

  bool result = false;
  nbdl::match(store, path, hana::overload_linearly(
    [&](Payload<2> const& result_payload)
    {
      result = hana::equal(result_payload, payload);
    }
  , nbdl::noop
  ));

  CHECK(result);
}

TEST_CASE("Apply message downstream update with value.", "[map_store][apply_message]")
{
  auto path = hana::make_tuple(PathKey<1>{11}, PathKey<2>{22});
  auto uid = FakeUid{242};
  auto payload = Payload<2>{42};

  auto store = make_test_store();
  store.map[path] = Payload<2>{123};

  // update always overwrites
  bool did_state_change = nbdl::apply_message(
    store
  , message::make_downstream_update(
      path
    , uid
    , payload
    , true
    )
  );

  CHECK(did_state_change == true);
  CHECK(store.map.size() == 1);

  bool result = false;
  nbdl::match(store, path, hana::overload_linearly(
    [&](Payload<2> const& result_payload)
    {
      result = hana::equal(result_payload, payload);
    }
  , nbdl::noop
  ));

  CHECK(result);
}

TEST_CASE("Apply message upstream delete.", "[map_store][apply_message]")
{
  auto path = hana::make_tuple(PathKey<1>{11}, PathKey<2>{22});
  auto uid = FakeUid{242};

  auto store = make_test_store();

  bool did_state_change = nbdl::apply_message(
    store
  , message::make_upstream_delete(
      path
    , uid
    )
  );

  CHECK(did_state_change == false);
  CHECK(store.map.size() == 0);
}

TEST_CASE("Apply message downstream delete.", "[map_store][apply_message]")
{
  auto path = hana::make_tuple(PathKey<1>{11}, PathKey<2>{22});
  auto uid = FakeUid{242};

  auto store = make_test_store();
  store.map[path] = Payload<2>{22};

  bool did_state_change = nbdl::apply_message(
    store
  , message::make_downstream_delete(
      path
    , uid
    , true
    )
  );

  CHECK(did_state_change == true);
  CHECK(store.map.size() == 0);
}

TEST_CASE("Apply messages to basic_store_map.", "[core]")
{
  auto store = nbdl::basic_map_store<int, int>{};
  nbdl::apply_message(store, message::make_downstream_update(
    42
  , message::no_uid
  , 421
  , true
  ));

  nbdl::apply_message(store, message::make_downstream_update(
    29
  , message::no_uid
  , 291
  , true
  ));

  nbdl::apply_message(store, message::make_downstream_update(
    14
  , message::no_uid
  , 141
  , true
  ));

  nbdl::apply_message(store, message::make_downstream_delete(
    14
  , message::no_uid
  , true
  ));

  {
    bool result = false;

    nbdl::match_when<int>(store, 42, [&](int x)
    {
      result = x == 421;
    });

    CHECK(result);
  }

  {
    bool result = false;

    nbdl::match_when<int>(store, 29, [&](int x)
    {
      result = x == 291;
    });

    CHECK(result);
  }

  {
    bool result = false;

    nbdl::match_when<nbdl::uninitialized>(store, 14, [&](nbdl::uninitialized)
    {
      result = true;
    });

    CHECK(result);
  }

  CHECK(store.map.size() == 2);
}
