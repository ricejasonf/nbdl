//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/apply_message.hpp>
#include <nbdl/make_context.hpp>
#include <nbdl/map_store.hpp>
#include <nbdl/match.hpp>
#include <nbdl/message.hpp>

#include <assets/TestContext.hpp>
#include <catch.hpp>

namespace hana = boost::hana;
using test_context::path;
using test_context::entity::my_entity;
namespace message = nbdl::message;

namespace test_context_match
{
  struct tcms_store_tag { };
  struct my_context { };

  template <typename Path>
  struct tcms_store
  {
    using hana_tag = tcms_store_tag;
  };
}

namespace nbdl
{
  template <>
  struct make_def_impl<test_context_match::my_context>
  {
    static constexpr auto apply()
    {
      return test_context_def::make(
        test_context::producer_tag{},
        test_context::producer_tag{},
        test_context::state_consumer_tag{},
        test_context::consumer_tag{},
        test_context_match::tcms_store_tag{}
      );
    }
  };

  template <>
  struct make_store_impl<test_context_match::tcms_store_tag>
  {
    template <typename PathType, typename EntityType>
    static constexpr auto apply(PathType, EntityType)
      -> test_context_match::tcms_store<typename PathType::type>
    { return {}; }
  };

  template <>
  struct match_impl<test_context_match::tcms_store_tag>
  {
    template <typename Store, typename Key, typename Fn>
    static constexpr void apply(Store&&, Key&& k, Fn&& fn)
    {
      if constexpr(decltype(hana::type_c<test_context::path<1>> == hana::typeid_(k)){})
      {
        if (hana::equal(k, test_context::path<1>(1, 49)))
        {
          std::forward<Fn>(fn)(
            test_context::entity::my_entity<1>{1, 49}
          );
        }
        else
        {
          std::forward<Fn>(fn)(nbdl::trigger_read{});
        }
      }
      else
      {
        std::forward<Fn>(fn)(nbdl::trigger_read{});
      }
    }
  };

  template <>
  struct apply_message_impl<test_context_match::tcms_store_tag>
  {
    template <typename Store, typename Message>
    static constexpr auto apply(Store&&, Message&&)
    {
      return hana::false_c;
    }
  };
}

TEST_CASE("Match a value in the stores.", "[context]")
{
  auto context = nbdl::make_unique_context<test_context_match::my_context>();
  auto& state_consumer  = context->actor<2>();

  bool result = false;

  nbdl::match(state_consumer.push_api, path<1>(1, 49), hana::overload_linearly(
    [&](my_entity<1> const& e)
    {
      result = hana::equal(e, my_entity<1>{1, 49});
    },
    [&](auto const&) { }
  ));

  CHECK(result);
}

TEST_CASE("Matching nbdl::trigger_read triggers upstream read message.", "[context]")
{
  auto context = nbdl::make_unique_context<test_context_match::my_context>();
  auto& producer1       = context->actor<1>();
  auto& state_consumer  = context->actor<2>();

  bool result = false;

  nbdl::match(state_consumer.push_api, path<1>(1, 13), hana::overload_linearly(
    [&](nbdl::unresolved)  { result = true; },
    [&](auto const&)       { result = false; }
  ));

  CHECK(result);

  // producer1 should record an upstream read message.
  REQUIRE(producer1.recorded_messages.size() == 1);
  {
    bool result = false;
    producer1.recorded_messages[0].match(
      [](auto const& x)
        -> std::enable_if_t<!nbdl::UpstreamMessage<decltype(x)>::value>
      { },
      [&](auto const& msg)
        ->  std::enable_if_t<decltype(hana::equal(
              hana::traits::decay(hana::decltype_(message::get_path(msg))),
              hana::decltype_(path<1>(1, 13))
            ))::value>
      {
        CHECK(message::is_read<decltype(msg)>);
        CHECK(hana::equal(message::get_path(msg), path<1>(1, 13)));
        result = true;
      },
      [](auto const&) { }
    );
    CHECK(result);
  }
}
