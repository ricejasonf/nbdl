//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/message.hpp>
#include <nbdl/uid.hpp>
#include <nbdl/variant.hpp>

#include <boost/hana/equal.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana/type.hpp>
#include <catch.hpp>

namespace hana = boost::hana;
namespace message = nbdl::message;

namespace
{
  using nbdl::message::channel::upstream;
  using nbdl::message::channel::downstream;
  using nbdl::message::action::create;
  using action_read = nbdl::message::action::read;
  using nbdl::message::action::update;
  using nbdl::message::action::delete_;

  template <typename T>
  constexpr auto decay_c(T)
  {
    return hana::type_c<std::decay_t<T>>;
  }

  template <int i>
  struct PathKey
  {
    int value;

    PathKey(int v)
      : value(v)
    { }
  };

  template <int i> struct Payload { int value; };

  struct FakeUid
  {
    int value;
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

TEST_CASE("upstream/downstream create message", "[message]")
{
  auto m = message::make_upstream_create(
    message::make_create_path<PathKey<2>>(hana::make_tuple(PathKey<1>{1}))
  , message::no_uid
  , Payload<2>{42}
  );

  static_assert(not message::is_path_normalizable<decltype(m)>);
  static_assert(not message::has_uid<decltype(m)>);
  static_assert(not message::has_is_confirmed<decltype(m)>);
  CHECK(hana::equal(message::get_payload(m), Payload<2>{42}));

  auto d1 = message::to_downstream_with_key(m, PathKey<2>{2});

  static_assert(message::is_path_normalizable<decltype(d1)>);
  static_assert(not message::has_uid<decltype(d1)>);
  static_assert(not message::has_is_confirmed<decltype(d1)>);
  CHECK(hana::equal(message::get_path(d1), hana::make_tuple(PathKey<1>{1}, PathKey<2>{2})));
  CHECK(hana::equal(message::get_path(d1), message::get_normalized_path(d1)));
  CHECK(hana::equal(message::get_payload(d1), Payload<2>{42}));
}

TEST_CASE("upstream/downstream create message with uid", "[message]")
{
  using Key = nbdl::variant<PathKey<2>, FakeUid>;

  auto m = message::make_upstream_create(
    message::make_create_path<Key>(hana::make_tuple(PathKey<1>{1}))
  , FakeUid{242}
  , Payload<2>{42}
  );

  static_assert(message::is_path_normalizable<decltype(m)>);
  static_assert(message::has_uid<decltype(m)>);
  CHECK(hana::equal(message::get_uid(m), FakeUid{242}));
  CHECK(hana::equal(message::get_payload(m), Payload<2>{42}));
  CHECK(hana::equal(message::get_normalized_path(m), hana::make_tuple(PathKey<1>{1}, Key{FakeUid{242}})));

  auto d1 = message::to_downstream_with_key_confirmed(m, PathKey<2>{2});
  using KeyVariant = nbdl::variant<PathKey<2>, FakeUid>;

  static_assert(message::is_path_normalizable<decltype(d1)>);
  static_assert(message::has_uid<decltype(d1)>);
  static_assert(message::has_is_confirmed<decltype(d1)>);
  CHECK(hana::equal(
    message::get_path(d1)
  , hana::make_tuple(PathKey<1>{1}, KeyVariant{PathKey<2>{2}}))
  );
  CHECK(hana::equal(message::get_path(d1), message::get_normalized_path(d1)));
  CHECK(hana::equal(message::get_uid(d1), FakeUid{242}));
  CHECK(hana::equal(message::get_payload(d1), Payload<2>{42}));
  CHECK(message::is_confirmed(d1));

  auto d2 = message::to_downstream(m, false);

  static_assert(message::is_path_normalizable<decltype(d2)>);
  static_assert(message::has_uid<decltype(d2)>);
  static_assert(message::has_is_confirmed<decltype(d2)>);
  CHECK(hana::equal(
    message::get_normalized_path(d2)
  , hana::make_tuple(PathKey<1>{1}, Key{FakeUid{242}})
  ));
  CHECK(hana::equal(message::get_uid(d2), FakeUid{242}));
  CHECK(hana::equal(message::get_payload(d2), Payload<2>{42}));
  CHECK(not message::is_confirmed(d2));
}

TEST_CASE("upstream/downstream read", "[message]")
{
  auto path = hana::make_tuple(PathKey<1>{1}, PathKey<2>{2});
  auto uid = FakeUid{242};
  auto payload = Payload<2>{42};

  auto m = message::make_upstream_read(
    path
  , uid
  );

  static_assert(not message::has_payload<decltype(m)>);
  static_assert(not message::has_payload<decltype(m)>);
  CHECK(hana::equal(message::get_path(m), path));
  CHECK(hana::equal(message::get_uid(m), uid));

  auto d1 = message::to_downstream_read(m, payload);

  static_assert(message::has_uid<decltype(d1)>);
  static_assert(message::has_payload<decltype(d1)>);
  CHECK(hana::equal(message::get_path(d1), path));
  CHECK(hana::equal(message::get_uid(d1), uid));
  CHECK(hana::equal(message::get_payload(d1), payload));

  // upstream read with no uid
  auto m2 = message::make_upstream_read(
    path
  , message::no_uid
  );

  static_assert(not message::has_uid<decltype(m2)>);
  static_assert(not message::has_payload<decltype(m2)>);
  CHECK(hana::equal(message::get_path(m2), path));

  auto d2 = message::to_downstream_read(
    m2
  , payload
  );

  static_assert(message::has_payload<decltype(d2)>);
  static_assert(not message::has_uid<decltype(d2)>);
  CHECK(hana::equal(message::get_path(d2), path));
  CHECK(hana::equal(message::get_payload(d2), payload));
}

TEST_CASE("upstream/downstream update", "[message]")
{
  auto path = hana::make_tuple(PathKey<1>{1}, PathKey<2>{2});
  auto uid = FakeUid{242};
  auto payload = Payload<2>{42};

  auto m = message::make_upstream_update(
    path
  , uid
  , payload
  );

  static_assert(message::has_uid<decltype(m)>);
  static_assert(message::has_payload<decltype(m)>);
  CHECK(hana::equal(message::get_path(m), path));
  CHECK(hana::equal(message::get_payload(m), payload));

  auto d1 = message::to_downstream(m , true);

  static_assert(message::has_uid<decltype(d1)>);
  static_assert(message::has_payload<decltype(d1)>);
  CHECK(hana::equal(message::get_path(d1), path));
  CHECK(hana::equal(message::get_payload(d1), payload));
  CHECK(message::is_confirmed(d1));

  auto d2 = message::to_downstream(m , false);
  CHECK(not message::is_confirmed(d2));
}

TEST_CASE("upstream/downstream delete", "[message]")
{
  auto path = hana::make_tuple(PathKey<1>{1}, PathKey<2>{2});
  auto uid = FakeUid{242};

  auto m = message::make_upstream_delete(
    path
  , uid
  );

  static_assert(message::has_uid<decltype(m)>);
  static_assert(not message::has_payload<decltype(m)>);
  CHECK(hana::equal(message::get_path(m), path));

  auto d1 = message::to_downstream(m , true);

  static_assert(message::has_uid<decltype(d1)>);
  static_assert(not message::has_payload<decltype(d1)>);
  CHECK(hana::equal(message::get_path(d1), path));
  CHECK(message::is_confirmed(d1));

  auto d2 = message::to_downstream(m , false);
  CHECK(not message::is_confirmed(d2));
}
