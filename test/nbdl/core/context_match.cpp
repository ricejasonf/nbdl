//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <assets/TestContext.hpp>
#include <nbdl/make_context.hpp>
#include <nbdl/map_store.hpp>
#include <nbdl/message.hpp>

#include <catch.hpp>

namespace hana = boost::hana;
using test_context::path;
using test_context::entity::my_entity;
namespace message = nbdl::message;

struct tcms_store_tag { };

template <typename Path>
struct tcms_store
{
  using hana_tag = tcms_store_tag;
};

namespace nbdl
{
  template <>
  struct make_store_impl<tcms_store_tag>
  {
    template <typename PathType>
    static constexpr auto apply(PathType)
      -> tcms_store<typename PathType::type>
    { return {}; }
  };

  template <>
  struct match_impl<tcms_store_tag>
  {
    template <typename Store, typename Path, typename ...Fns>
    static constexpr decltype(auto) apply(Store&&, Path&& p, Fns&& ...fns)
    {
      return hana::overload_linearly(
        [&](test_context::path<1> const& p)
        {
          if (hana::equal(p, test_context::path<1>(1, 49)))
          {
            return hana::overload_linearly(std::forward<Fns>(fns)...)
              (test_context::entity::my_entity<1>{1, 49});
          }
          else
          {
            return hana::overload_linearly(std::forward<Fns>(fns)...)
              (nbdl::uninitialized{});
          }
        },
        [&](auto const&)
        {
          return hana::overload_linearly(std::forward<Fns>(fns)...)
            (nbdl::uninitialized{});
        }
      )(std::forward<Path>(p));
    }
  };

  template <>
  struct apply_action_impl<tcms_store_tag>
  {
    template <typename Store, typename Message>
    static constexpr auto apply(Store&&, Message&&)
    {
      return hana::false_c;
    }
  };
}

constexpr auto def = test_context_def::make(
  test_context::provider_tag{},
  test_context::provider_tag{},
  test_context::state_consumer_tag{},
  test_context::consumer_tag{},
  tcms_store_tag{}
);
namespace
{
  auto context = nbdl::make_unique_context(def);
  auto& provider0       = context->cell<0>();
  auto& provider1       = context->cell<1>();
  auto& state_consumer  = context->cell<2>();
}

TEST_CASE("Match a value in the stores.", "[context]")
{
  bool result = state_consumer.push_api.match(path<1>(1, 49),
    [](my_entity<1> const& e)
    {
      return hana::equal(e, my_entity<1>{1, 49});
    },
    [](auto const&) { return false; }
  );
  CHECK(result);
}

TEST_CASE("Matching nbdl::uninitialized triggers upstream read message.", "[context]")
{
  bool result = state_consumer.push_api.match(path<1>(1, 13),
    [](nbdl::unresolved)  { return true; },
    [](auto const&)       { return false; }
  );
  CHECK(result);

  // provider1 should record an upstream read message.
  REQUIRE(provider1.recorded_messages.size() == 1);
  {
    bool result = provider1.recorded_messages[0].match(
      [](auto const& x)
        -> std::enable_if_t<!nbdl::UpstreamMessage<decltype(x)>::value, bool>
      { return false; },
      [](auto const& msg)
        ->  std::enable_if_t<decltype(hana::equal(
              hana::traits::decay(hana::decltype_(message::get_path(msg))),
              hana::decltype_(path<1>(1, 13))
            ))::value, bool>
      {
        CHECK(message::is_read<decltype(msg)>);
        CHECK(hana::equal(message::get_path(msg), path<1>(1, 13)));
        return true;
      },
      [](auto const&) { return false; }
    );
    CHECK(result);
  }
}
