//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/message_api.hpp>
#include <nbdl/uid.hpp>

#include <boost/hana/experimental/types.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana/type.hpp>
#include <catch.hpp>

namespace hana = boost::hana;
namespace hanax = boost::hana::experimental;
namespace message = nbdl::message;

namespace
{
  using nbdl::message::channel::upstream;
  using nbdl::message::channel::downstream;
  using nbdl::message::action::create;
  using action_read = nbdl::message::action::read;
  using nbdl::message::action::update;
  using nbdl::message::action::delete_;
  using maybe_uid = hana::optional<nbdl::uid>;

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
  template <int i> using Path = hana::tuple<PathKey<i>>;
  template <int i> using CreatePath = hana::tuple<hana::type<PathKey<i>>>;
  template <int i> using MaybePayload = hana::optional<Payload<i>>;
}

namespace boost { namespace hana {
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
}} // boost::hana

namespace
{
  // TODO create a better way for making a message type for testing (ie named params)
  template <int i>
  using UpstreamCreate = hana::tuple<upstream, create,
    CreatePath<i>,
    maybe_uid, MaybePayload<i>>;
  template <int i>
  using UpstreamRead = hana::tuple<upstream, action_read, Path<i>, maybe_uid>;
  template <int i>
  using UpstreamUpdate = hana::tuple<upstream, update, Path<i>, maybe_uid, MaybePayload<i>>;
  template <int i>
  using UpstreamDelete = hana::tuple<upstream, delete_, Path<i>, maybe_uid>;
  template <int i>
  using DownstreamCreate = hana::tuple<downstream, create, Path<i>, maybe_uid,
    hana::optional<bool>, MaybePayload<i>>;
  template <int i>
  using DownstreamRead = hana::tuple<downstream, action_read, Path<i>, maybe_uid,
    hana::optional<bool>, MaybePayload<i>>;
  template <int i>
  using DownstreamUpdate = hana::tuple<downstream, update, Path<i>, maybe_uid,
    hana::optional<bool>, MaybePayload<i>>;
  template <int i>
  using DownstreamDelete = hana::tuple<downstream, delete_, Path<i>, maybe_uid, hana::optional<bool>>;

  using UpstreamMessages = hanax::types<
      UpstreamCreate<1>
    , UpstreamRead<1>
    , UpstreamCreate<2>
    , UpstreamUpdate<2>
    , UpstreamCreate<3>
    , UpstreamRead<3>
    , UpstreamUpdate<3>
    , UpstreamDelete<3>
    >;
  using DownstreamMessages = hanax::types<
      DownstreamCreate<1>
    , DownstreamRead<1>
    , DownstreamCreate<2>
    , DownstreamUpdate<2>
    , DownstreamCreate<3>
    , DownstreamRead<3>
    , DownstreamUpdate<3>
    , DownstreamDelete<3>
    >;

  struct context_mock
  {
    struct message_api_meta
    {
      using upstream_types    = UpstreamMessages;
      using downstream_types  = DownstreamMessages;
    };
  };

  constexpr nbdl::message_api<context_mock> msgs{};
}

// Test the search for message types

BOOST_HANA_CONSTANT_ASSERT(
  msgs.get_message_type(upstream{}, create{}, CreatePath<1>{}) == hana::type_c<UpstreamCreate<1>>
);
BOOST_HANA_CONSTANT_ASSERT(
  msgs.get_message_type(upstream{}, action_read{}, Path<1>{1}) == hana::type_c<UpstreamRead<1>>
);
BOOST_HANA_CONSTANT_ASSERT(
  msgs.get_message_type(downstream{}, create{}, Path<1>{1}) == hana::type_c<DownstreamCreate<1>>
);
BOOST_HANA_CONSTANT_ASSERT(
  msgs.get_message_type(downstream{}, action_read{}, Path<1>{1}) == hana::type_c<DownstreamRead<1>>
);
BOOST_HANA_CONSTANT_ASSERT(
  msgs.get_message_type(upstream{}, update{}, Path<2>{2}) == hana::type_c<UpstreamUpdate<2>>
);
BOOST_HANA_CONSTANT_ASSERT(
  msgs.get_message_type(upstream{}, delete_{}, Path<3>{3}) == hana::type_c<UpstreamDelete<3>>
);

TEST_CASE("make_upstream_create_message", "[message_api]")
{
  auto m = msgs.make_upstream_create_message(CreatePath<1>{}, Payload<1>{11});
  BOOST_HANA_CONSTANT_ASSERT(decay_c(message::get_channel(m)) == hana::type_c<upstream>);
  BOOST_HANA_CONSTANT_ASSERT(decay_c(message::get_action(m)) == hana::type_c<create>);
  CHECK(hana::equal(message::get_path(m), CreatePath<1>{}));
  CHECK(hana::equal(*message::get_maybe_payload(m), Payload<1>{11}));
}

TEST_CASE("make_downstream_create_message", "[message_api]")
{
  auto m = msgs.make_downstream_create_message(Path<1>{1}, Payload<1>{11});
  BOOST_HANA_CONSTANT_ASSERT(decay_c(message::get_channel(m)) == hana::type_c<downstream>);
  BOOST_HANA_CONSTANT_ASSERT(decay_c(message::get_action(m)) == hana::type_c<create>);
  CHECK(hana::equal(message::get_path(m), Path<1>{1}));
  CHECK(hana::equal(*message::get_maybe_payload(m), Payload<1>{11}));
  CHECK(*message::get_maybe_is_from_root(m) == false);
}

TEST_CASE("make_upstream_read_message", "[message_api]")
{
  auto m = msgs.make_upstream_read_message(Path<1>{1});
  BOOST_HANA_CONSTANT_ASSERT(decay_c(message::get_channel(m)) == hana::type_c<upstream>);
  BOOST_HANA_CONSTANT_ASSERT(decay_c(message::get_action(m)) == hana::type_c<action_read>);
  CHECK(hana::equal(message::get_path(m), Path<1>{1}));
}

TEST_CASE("make_upstream_update_message", "[message_api]")
{
  auto m = msgs.make_upstream_update_message(Path<3>{3}, Payload<3>{33});
  BOOST_HANA_CONSTANT_ASSERT(decay_c(message::get_channel(m)) == hana::type_c<upstream>);
  BOOST_HANA_CONSTANT_ASSERT(decay_c(message::get_action(m)) == hana::type_c<update>);
  CHECK(hana::equal(message::get_path(m), Path<3>{3}));
  CHECK(hana::equal(*message::get_maybe_payload(m), Payload<3>{33}));
}

TEST_CASE("make_downstream_update_message", "[message_api]")
{
  auto m = msgs.make_downstream_update_message(Path<3>{3}, Payload<3>{33});
  BOOST_HANA_CONSTANT_ASSERT(decay_c(message::get_channel(m)) == hana::type_c<downstream>);
  BOOST_HANA_CONSTANT_ASSERT(decay_c(message::get_action(m)) == hana::type_c<update>);
  CHECK(hana::equal(message::get_path(m), Path<3>{3}));
  CHECK(hana::equal(*message::get_maybe_payload(m), Payload<3>{33}));
  CHECK(*message::get_maybe_is_from_root(m) == false);
}

TEST_CASE("make_upstream_delete_message", "[message_api]")
{
  auto m = msgs.make_upstream_delete_message(Path<3>{3});
  BOOST_HANA_CONSTANT_ASSERT(decay_c(message::get_channel(m)) == hana::type_c<upstream>);
  BOOST_HANA_CONSTANT_ASSERT(decay_c(message::get_action(m)) == hana::type_c<delete_>);
  CHECK(hana::equal(message::get_path(m), Path<3>{3}));
}

TEST_CASE("make_downstream_delete_message", "[message_api]")
{
  auto m = msgs.make_downstream_delete_message(Path<3>{3});
  BOOST_HANA_CONSTANT_ASSERT(decay_c(message::get_channel(m)) == hana::type_c<downstream>);
  BOOST_HANA_CONSTANT_ASSERT(decay_c(message::get_action(m)) == hana::type_c<delete_>);
  CHECK(hana::equal(message::get_path(m), Path<3>{3}));
  CHECK(*message::get_maybe_is_from_root(m) == false);
}

#if 0
// TODO: test with a path that has a uid
// This test must use a path that has a uid or you can't
// convert it to downstream unless it is from root such
// that a value is provided for the key.
TEST_CASE("to_downstream (create)", "[message_api]")
{
  auto m = msgs.make_upstream_create_message(CreatePath<1>{}, Payload<1>{11});
  auto down1 = msgs.to_downstream(m);
  auto down2 = msgs.to_downstream(m, Payload<1>{111}); // different payload
  BOOST_HANA_CONSTANT_ASSERT(hana::decltype_(down1) == hana::type_c<DownstreamCreate<1>>);
  BOOST_HANA_CONSTANT_ASSERT(hana::decltype_(down2) == hana::type_c<DownstreamCreate<1>>);
  CHECK(hana::equal(message::get_path(down1), Path<1>{1}));
  CHECK(hana::equal(message::get_path(down2), Path<1>{1}));
  CHECK(hana::equal(*message::get_maybe_payload(down1), Payload<1>{11}));
  CHECK(hana::equal(*message::get_maybe_payload(down2), Payload<1>{111}));
  CHECK(*message::get_maybe_is_from_root(down1) == false);
  CHECK(*message::get_maybe_is_from_root(down2) == false);
}
#endif

TEST_CASE("to_downstream (read)", "[message_api]")
{
  auto m = msgs.make_upstream_read_message(Path<1>{1});
  auto down = msgs.to_downstream(m, Payload<1>{11});
  BOOST_HANA_CONSTANT_ASSERT(hana::decltype_(down) == hana::type_c<DownstreamRead<1>>);
  CHECK(hana::equal(message::get_path(down), Path<1>{1}));
  CHECK(hana::equal(*message::get_maybe_payload(down), Payload<1>{11}));
}

TEST_CASE("to_downstream (update)", "[message_api]")
{
  auto m = msgs.make_upstream_update_message(Path<3>{3}, Payload<3>{33});
  auto down3 = msgs.to_downstream(m);
  auto down2 = msgs.to_downstream(m, Payload<3>{333}); // different payload
  BOOST_HANA_CONSTANT_ASSERT(hana::decltype_(down3) == hana::type_c<DownstreamUpdate<3>>);
  BOOST_HANA_CONSTANT_ASSERT(hana::decltype_(down2) == hana::type_c<DownstreamUpdate<3>>);
  CHECK(hana::equal(message::get_path(down3), Path<3>{3}));
  CHECK(hana::equal(message::get_path(down2), Path<3>{3}));
  CHECK(hana::equal(*message::get_maybe_payload(down3), Payload<3>{33}));
  CHECK(hana::equal(*message::get_maybe_payload(down2), Payload<3>{333}));
}

TEST_CASE("to_downstream (delete)", "[message_api]")
{
  auto m = msgs.make_upstream_delete_message(Path<3>{3});
  auto down = msgs.to_downstream(m);
  BOOST_HANA_CONSTANT_ASSERT(hana::decltype_(down) == hana::type_c<DownstreamDelete<3>>);
  CHECK(hana::equal(message::get_path(down), Path<3>{3}));
}

TEST_CASE("to_downstream_from_root (create)", "[message_api]")
{
  auto m = msgs.make_upstream_create_message(CreatePath<1>{}, Payload<1>{11});
  auto down1 = msgs.to_downstream_from_root(m, 1);
  auto down2 = msgs.to_downstream_from_root_with_payload(m, 1, Payload<1>{111}); // different payload
  BOOST_HANA_CONSTANT_ASSERT(hana::decltype_(down1) == hana::type_c<DownstreamCreate<1>>);
  BOOST_HANA_CONSTANT_ASSERT(hana::decltype_(down2) == hana::type_c<DownstreamCreate<1>>);
  CHECK(hana::equal(message::get_path(down1), Path<1>{1}));
  CHECK(hana::equal(message::get_path(down2), Path<1>{1}));
  CHECK(hana::equal(*message::get_maybe_payload(down1), Payload<1>{11}));
  CHECK(hana::equal(*message::get_maybe_payload(down2), Payload<1>{111}));
  CHECK(*message::get_maybe_is_from_root(down1) == true);
  CHECK(*message::get_maybe_is_from_root(down2) == true);
}

TEST_CASE("to_downstream_from_root with nested path (create)", "[message_api]")
{
  using Path_ = hana::tuple<PathKey<1>, PathKey<2>>;
  using CreatePath_ = hana::tuple<PathKey<1>, hana::type<PathKey<2>>>;
  using UpstreamCreateMessage = hana::tuple<
    upstream,
    create,
    CreatePath_,
    maybe_uid,
    MaybePayload<2>
  >;
  using DownstreamCreateMessage = hana::tuple<downstream, create,
    Path_, maybe_uid, hana::optional<bool>, MaybePayload<2>
  >;
  struct context_mock
  {
    struct message_api_meta
    {
      using upstream_types    = hanax::types<UpstreamCreateMessage>;
      using downstream_types  = hanax::types<DownstreamCreateMessage>;
    };
  };

  constexpr nbdl::message_api<context_mock> msgs{};

  auto m = msgs.make_upstream_create_message(
    CreatePath_(PathKey<1>{1}, hana::type_c<PathKey<2>>),
    Payload<2>{22}
  );
  auto down1 = msgs.to_downstream_from_root(m, PathKey<2>{2});
  auto down2 = msgs.to_downstream_from_root_with_payload(
    m,
    PathKey<2>{2},
    Payload<2>{222} // different payload
  );
  BOOST_HANA_CONSTANT_ASSERT(hana::typeid_(down1) == hana::type_c<DownstreamCreateMessage>);
  BOOST_HANA_CONSTANT_ASSERT(hana::typeid_(down2) == hana::type_c<DownstreamCreateMessage>);
  CHECK(hana::equal(message::get_path(down1), Path_(PathKey<1>{1}, PathKey<2>{2})));
  CHECK(hana::equal(message::get_path(down2), Path_(PathKey<1>{1}, PathKey<2>{2})));
  CHECK(hana::equal(message::get_payload(down1), Payload<2>{22}));
  CHECK(hana::equal(message::get_payload(down2), Payload<2>{222}));
  CHECK(message::get_is_from_root(down1) == true);
  CHECK(message::get_is_from_root(down2) == true);
}
