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

using nbdl::message::channel::upstream;
using nbdl::message::channel::downstream;
using nbdl::message::action::create;
using nbdl::message::action::read;
using nbdl::message::action::update;
using nbdl::message::action::update_raw;
using nbdl::message::action::delete_;
using nbdl::uid;

template <int i> using Path             = hana::tuple<hana::int_c<i>, int>;
template <int i> using Payload          = hana::tuple<hana::type_c<Path<i>>, int>;
template <int i> using UpstreamCreate   = hana::tuple<upstream, create, uid, Path<i>, Payload<i>>;
template <int i> using UpstreamRead     = hana::tuple<upstream, read, uid, Path<i>>;
template <int i> using UpstreamUpdate   = hana::tuple<upstream, update, uid, Path<i>, Payload<i>>;
template <int i> using UpstreamDelete   = hana::tuple<upstream, delete_, uid, Path<i>>;
template <int i> using DownstreamCreate = hana::tuple<downstream, create, uid, bool, Path<i>, Payload<i>>;
template <int i> using DownstreamRead   = hana::tuple<downstream, read, uid, bool, Path<i>, Payload<i>>;
template <int i> using DownstreamUpdate = hana::tuple<downstream, update, uid, bool, Path<i>, Payload<i>>;
template <int i> using DownstreamDelete = hana::tuple<downstream, delete_, uid, bool, Path<i>>;

using UpstreamMessages = hana::tuple<
    UpstreamCreate<1>
  , UpstreamRead<1>
  , UpstreamCreate<2>
  , UpstreamUpdate<2>
  , UpstreamCreate<3>
  , UpstreamRead<3>
  , UpstreamUpdate<3>
  , UpstreamDelete<3>
  >;
using DownstreamMessages = hana::tuple<
    DownstreamCreate<1>
  , DownstreamRead<1>
  , DownstreamCreate<2>
  , DownstreamUpdate<2>
  , DownstreamCreate<3>
  , DownstreamRead<3>
  , DownstreamUpdate<3>
  , DownstreamDelete<3>
  >;

TEST_CASE("", "[message_api]")
{
}
