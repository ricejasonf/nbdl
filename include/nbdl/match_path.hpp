//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MATCH_PATH_HPP
#define NBDL_MATCH_PATH_HPP

#include <nbdl/fwd/match_path.hpp>

#include <nbdl/concept/Store.hpp>
#include <nbdl/match.hpp>

#include <boost/hana/drop_front.hpp>
#include <boost/hana/front.hpp>
#include <boost/hana/functional/overload_linearly.hpp>
#include <utility>

namespace nbdl
{
  namespace detail
  {
    template <typename Path, typename Fn>
    struct match_path_helper_fn
    {
      Path const& path;
      Fn const& fn;

      template <typename Value>
      constexpr void operator()(Value&& value) const
      {
        if constexpr(nbdl::Store<decltype(value)>::value)
        {
          match_path(
            std::forward<Value>(value)
          , hana::drop_front(path)
          , fn
          );
        }
        else
        {
          fn(std::forward<Value>(value));
        }
      }
    };
  }

  template<typename Store, typename Path, typename ...Fns>
  constexpr void match_path_fn::operator()(Store&& s, Path&& p, Fns&&... fns) const
  {
    auto fn = hana::overload_linearly(std::forward<Fns>(fns)...);

    static_assert(
      hana::Sequence<Path>::value
    , "nbdl::match_path(store, path, fns...) requires path must be a hana::Sequence"
    );

    if constexpr(not decltype(hana::is_empty(p))::value)
    {
      auto helper = detail::match_path_helper_fn<Path, decltype(fn)>{p, std::move(fn)};

      nbdl::match(
        std::forward<Store>(s)
      , hana::front(std::forward<Path>(p))
      , std::move(helper)
      );
    }
    else if constexpr(nbdl::Store<Store>::value)
    {
      nbdl::match(
        std::forward<Store>(s)
      , std::move(fn)
      );
    }
    else
    {
      // When path is empty match behaves like an identity operation
      // and does not require the `s` to be a Store
      // static_assert(not nbdl::Store<Store>::value);
      // static_assert(decltype(hana::is_empty(p))::value);

      fn(std::forward<Store>(s));
    }
  };
}

#endif
