//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <assets/TestContext.hpp>
#include <nbdl/echo_producer.hpp>
#include <nbdl/make_context.hpp>
#include <nbdl/message.hpp>
#include <nbdl/null_store.hpp>

#include <catch.hpp>

namespace message = nbdl::message;

namespace test_echo_producer
{
  struct my_context { };
}

namespace nbdl
{
  template <>
  struct make_def_impl<test_echo_producer::my_context>
  {
    static constexpr auto apply()
    {
      return test_context_def::make(
        nbdl::echo_producer{},
        test_context::producer_tag{},
        test_context::consumer_tag{},
        test_context::consumer_tag{},
        nbdl::null_store{}
      );
    }
  };
}

TEST_CASE("Echo producer should echo messages to downstream.", "[echo_producer]")
{
  auto context = nbdl::make_unique_context<test_echo_producer::my_context>();
  auto& consumer2 = context->cell<2>();

  using Path = test_context::path<0>;
  using Entity = test_context::entity::my_entity<1>;

  auto msg = message::make_upstream_create(
    message::make_create_path<test_context::key<test_context::entity::my_entity<1>>>(
      hana::make_tuple(test_context::key<test_context::entity::root1>{5})
    )
  , message::no_uid
  , Entity{5, 5}
  );
  consumer2.push_api.push(msg);
  consumer2.push_api.push(msg);
  consumer2.push_api.push(msg);

  REQUIRE(consumer2.recorded_messages.size() == 3);

  consumer2.recorded_messages[0].match(
    [](auto const& m) -> std::enable_if_t<!nbdl::DownstreamMessage<decltype(m)>::value>
    { CHECK(false); },
    [](auto const& m) -> std::enable_if_t<
      decltype(hana::equal(message::get_path_type(m), hana::type_c<Path>))::value>
    { CHECK(hana::equal(message::get_path(m), Path(5, 1))); },
    [](auto const&)
    { CHECK(false); }
  );

  consumer2.recorded_messages[1].match(
    [](auto const& m) -> std::enable_if_t<!nbdl::DownstreamMessage<decltype(m)>::value>
    { CHECK(false); },
    [](auto const& m) -> std::enable_if_t<
      decltype(hana::equal(message::get_path_type(m), hana::type_c<Path>))::value>
    { CHECK(hana::equal(message::get_path(m), Path(5, 2))); },
    [](auto const&)
    { CHECK(false); }
  );

  consumer2.recorded_messages[2].match(
    [](auto const& m) -> std::enable_if_t<!nbdl::DownstreamMessage<decltype(m)>::value>
    { CHECK(false); },
    [](auto const& m) -> std::enable_if_t<
      decltype(hana::equal(message::get_path_type(m), hana::type_c<Path>))::value>
    { CHECK(hana::equal(message::get_path(m), Path(5, 3))); },
    [](auto const&)
    { CHECK(false); }
  );
}
