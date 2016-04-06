//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/message_api.hpp>
#include <Uid.hpp> // FIXME

#include <boost/hana/tuple.hpp>
#include <catch.hpp>

namespace message = nbdl::message;

using nbdl::message::channel::upstream;
using nbdl::message::channel::downstream;
using nbdl::message::action::create;
using nbdl::message::action::read;
using nbdl::message::action::update;
using nbdl::message::action::update_raw;
using nbdl::message::action::delete_;
using nbdl::uid;

template <int i> struct Path { int id; };
template <int i> struct Payload { int value; };

namespace boost { namespace hana {
  template <int i>
  hana::equal_impl<Path<i>, Path<i>>
  { static constexpr auto apply(T1 const& t1, T2 const& t2) { return t1.id = t2.id } };

  template <int i>
  hana::equal_impl<Payload<i>, Payload<i>>
  { static constexpr auto apply(T1 const& t1, T2 const& t2) { return t1.value = t2.value } };
}} // boost::hana

template <int i> using UpstreamCreate   = hana::tuple<upstream, create, uid, Path<i>, Payload<i>>;
template <int i> using UpstreamRead     = hana::tuple<upstream, read, uid, Path<i>>;
template <int i> using UpstreamUpdate   = hana::tuple<upstream, update, uid, Path<i>, Payload<i>>;
template <int i> using UpstreamUpdateRaw = hana::tuple<upstream, update_raw, uid, Path<i>, Payload<i>>;
template <int i> using UpstreamDelete   = hana::tuple<upstream, delete_, uid, Path<i>>;
template <int i> using DownstreamCreate = hana::tuple<downstream, create, uid, bool, Path<i>, Payload<i>>;
template <int i> using DownstreamRead   = hana::tuple<downstream, read, uid, bool, Path<i>, Payload<i>>;
template <int i> using DownstreamUpdate = hana::tuple<downstream, update, uid, bool, Path<i>, Payload<i>>;
template <int i> using DownstreamUpdateRaw
  = hana::tuple<downstream, update_raw, uid, bool, Path<i>, Payload<i>>;
template <int i> using DownstreamDelete = hana::tuple<downstream, delete_, uid, bool, Path<i>>;

using UpstreamMessages = hana::tuple<
    UpstreamCreate<1>
  , UpstreamRead<1>
  , UpstreamCreate<2>
  , UpstreamUpdateRaw<2>
  , UpstreamCreate<3>
  , UpstreamRead<3>
  , UpstreamUpdate<3>
  , UpstreamDelete<3>
  >;
using DownstreamMessages = hana::tuple<
    DownstreamCreate<1>
  , DownstreamRead<1>
  , DownstreamCreate<2>
  , DownstreamUpdateRaw<2>
  , DownstreamCreate<3>
  , DownstreamRead<3>
  , DownstreamUpdate<3>
  , DownstreamDelete<3>
  >;

constexpr nbdl::message_api<UpstreamMessages, DownstreamMessages> msgs{};

// Test the search for message types

BOOST_HANA_CONSTANT_ASSERT(
  msgs.get_message_type(upstream, create, Path<1>) == hana::type_c<UpstreamCreate<1>>
);
BOOST_HANA_CONSTANT_ASSERT(
  msgs.get_message_type(upstream, read, Path<1>) == hana::type_c<UpstreamRead<1>>
);
BOOST_HANA_CONSTANT_ASSERT(
  msgs.get_message_type(downstream, create, Path<1>) == hana::type_c<UpstreamCreate<1>>
);
BOOST_HANA_CONSTANT_ASSERT(
  msgs.get_message_type(downstream, read, Path<1>) == hana::type_c<UpstreamRead<1>>
);
BOOST_HANA_CONSTANT_ASSERT(
  msgs.get_message_type(upstream, update, Path<2>) == hana::type_c<UpstreamUpdate<2>>
);
BOOST_HANA_CONSTANT_ASSERT(
  msgs.get_message_type(upstream, delete_, Path<2>) == hana::type_c<UpstreamDelete<2>>
);

TEST_CASE("make_upstream_create_message", "[message_api]")
{
  auto m = msgs.make_upstream_create_message(Path<1>{}, Payload<11>{});
  BOOST_HANA_CONSTANT_ASSERT(hana::type_c<decltype(hana::get_channel(m))> == hana::type_c<upstream>);
  BOOST_HANA_CONSTANT_ASSERT(hana::type_c<decltype(hana::get_action(m))> == hana::type_c<create>);
  CHECK(hana::equal(message::get_path(m), Path<1>{1}));
  CHECK(hana::equal(message::get_payload(m), Payload<1>{11}));
}

TEST_CASE("make_downstream_create_message", "[message_api]")
{
  auto result = msgs.make_downstream_create_message(Path<1>{}, Payload<11>{});
  BOOST_HANA_CONSTANT_ASSERT(hana::type_c<decltype(hana::get_channel(m))> == hana::type_c<downstream>);
  BOOST_HANA_CONSTANT_ASSERT(hana::type_c<decltype(hana::get_action(m))> == hana::type_c<create>);
  CHECK(hana::equal(message::get_path(m), Path<1>{1}));
  CHECK(hana::equal(message::get_payload(m), Payload<1>{11}));
  CHECK(message::is_from_root(m) == false)
}

TEST_CASE("make_upstream_read_message", "[message_api]")
{
  auto result = msgs.make_upstream_read_message(Path<1>{});
  BOOST_HANA_CONSTANT_ASSERT(hana::type_c<decltype(hana::get_channel(m))> == hana::type_c<upstream>);
  BOOST_HANA_CONSTANT_ASSERT(hana::type_c<decltype(hana::get_action(m))> == hana::type_c<read>);
  CHECK(hana::equal(message::get_path(m), Path<1>{1}));
}

TEST_CASE("make_upstream_update_message", "[message_api]")
{
  auto result = msgs.make_upstream_update_message(Path<3>{}, Payload<33>{});
  BOOST_HANA_CONSTANT_ASSERT(hana::type_c<decltype(hana::get_channel(m))> == hana::type_c<upstream>);
  BOOST_HANA_CONSTANT_ASSERT(hana::type_c<decltype(hana::get_action(m))> == hana::type_c<update>);
  CHECK(hana::equal(message::get_path(m), Path<3>{3}));
  CHECK(hana::equal(message::get_payload(m), Payload<3>{33}));
}

TEST_CASE("make_downstream_update_message", "[message_api]")
{
  auto result = msgs.make_downstream_update_message(Path<3>{}, Payload<3>{});
  BOOST_HANA_CONSTANT_ASSERT(hana::type_c<decltype(hana::get_channel(m))> == hana::type_c<downstream>);
  BOOST_HANA_CONSTANT_ASSERT(hana::type_c<decltype(hana::get_action(m))> == hana::type_c<update>);
  CHECK(hana::equal(message::get_path(m), Path<3>{3}));
  CHECK(hana::equal(message::get_payload(m), Payload<3>{33}));
  CHECK(message::is_from_root(m) == false)
}

TEST_CASE("make_upstream_update_raw_message", "[message_api]")
{
  auto result = msgs.make_upstream_update_raw_message(Path<2>{}, Payload<2>{});
  BOOST_HANA_CONSTANT_ASSERT(hana::type_c<decltype(hana::get_channel(m))> == hana::type_c<upstream>);
  BOOST_HANA_CONSTANT_ASSERT(hana::type_c<decltype(hana::get_action(m))> == hana::type_c<update_raw>);
  CHECK(hana::equal(message::get_path(m), Path<2>{2}));
  CHECK(hana::equal(message::get_payload(m), Payload<2>{22}));
}

TEST_CASE("make_downstream_update_raw_message", "[message_api]")
{
  auto result = msgs.make_downstream_update_raw_message(Path<2>{}, Payload<2>{});
  BOOST_HANA_CONSTANT_ASSERT(hana::type_c<decltype(hana::get_channel(m))> == hana::type_c<downstream>);
  BOOST_HANA_CONSTANT_ASSERT(hana::type_c<decltype(hana::get_action(m))> == hana::type_c<update_raw>);
  CHECK(hana::equal(message::get_path(m), Path<2>{2}));
  CHECK(hana::equal(message::get_payload(m), Payload<2>{22}));
  CHECK(message::is_from_root(m) == false)
}

TEST_CASE("make_upstream_delete_message", "[message_api]")
{
  auto result = msgs.make_upstream_delete_message(Path<3>{}, Payload<3>{});
  BOOST_HANA_CONSTANT_ASSERT(hana::type_c<decltype(hana::get_channel(m))> == hana::type_c<upstream>);
  BOOST_HANA_CONSTANT_ASSERT(hana::type_c<decltype(hana::get_action(m))> == hana::type_c<delete_>);
  CHECK(hana::equal(message::get_path(m), Path<3>{3}));
}

TEST_CASE("make_downstream_delete_message", "[message_api]")
{
  auto result = msgs.make_downstream_delete_message(Path<3>{}, Payload<3>{});
  BOOST_HANA_CONSTANT_ASSERT(hana::type_c<decltype(hana::get_channel(m))> == hana::type_c<downstream>);
  BOOST_HANA_CONSTANT_ASSERT(hana::type_c<decltype(hana::get_action(m))> == hana::type_c<delete_>);
  CHECK(hana::equal(message::get_path(m), Path<3>{3}));
  CHECK(message::is_from_root(m) == false)
}
