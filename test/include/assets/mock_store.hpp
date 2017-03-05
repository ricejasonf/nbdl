//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NBDL_TEST_ASSETS_MOCK_STORE_HPP
#define NBDL_TEST_ASSETS_MOCK_STORE_HPP

#include <boost/hana.hpp>
#include <utility>
#include <vector>

#include <assets/TestContext.hpp>

namespace test_context
{
  struct mock_store_tag { };

  template <typename Path, typename Entity>
  struct mock_store
  {
    using hana_tag = mock_store_tag;
    using path = Path;
    using entity = Entity;
  };
}

namespace
{
  using test_context::path_variant;
  path_variant mock_store_result_apply_action;
  hana::tuple<
    path_variant,             // path of store that is listening
    path_variant,             // path of message to listen for
    std::vector<path_variant> // paths that will be marked as changed as a result of the message
  >  mock_store_result_apply_foreign_action;
}

namespace nbdl
{
  // Store - mock_store

  template <>
  struct make_store_impl<test_context::mock_store_tag>
  {
    template <typename PathType, typename EntityType>
    static constexpr auto apply(PathType, EntityType)
      -> test_context::mock_store<typename PathType::type, typename EntityType::type>
    { return {}; }
  };

  template <>
  struct get_impl<test_context::mock_store_tag>
  {
    template <typename Store, typename Path>
    static constexpr auto apply(Store const&, Path const&)
      -> typename Store::entity
    { return {}; }
  };

  template <>
  struct apply_action_impl<test_context::mock_store_tag>
  {
    template <typename Store, typename Message>
    static constexpr auto apply(Store&&, Message const& m)
    {
      using Path = std::decay_t<decltype(message::get_path(m))>;
      return mock_store_result_apply_action.match(
        [&](Path const& p)
        {
          return hana::equal(p, message::get_path(m));
        },
        [](auto const&) { return false; }
      );
    }
  };

  template <>
  struct apply_foreign_action_impl<test_context::mock_store_tag>
  {
    template <typename Store, typename Message, typename Fn>
    static constexpr auto apply(Store const&, Message const& m, Fn const& fn)
    {
      using Path = typename std::decay_t<Store>::path;
      using MessagePath = typename std::decay_t<decltype(message::get_path(m))>;
      hana::at_c<0>(mock_store_result_apply_foreign_action).match([&](Path const&)
      {
        hana::at_c<1>(mock_store_result_apply_foreign_action).match([&](MessagePath const& mp)
        {
          if (hana::equal(mp, message::get_path(m)))
          {
            for (auto const& v : hana::at_c<2>(mock_store_result_apply_foreign_action))
            {
              v.match([&](Path const& p_)
              {
                fn(p_);
              }, nbdl::noop);
            }
          }
        }, nbdl::noop);
      }, nbdl::noop);
    }
  };
}

#endif
