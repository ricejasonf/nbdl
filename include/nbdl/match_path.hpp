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
#include <nbdl/concept/Path.hpp>
#include <nbdl/match.hpp>

#include <boost/hana/drop_front.hpp>
#include <boost/hana/front.hpp>
#include <boost/hana/functional/overload_linearly.hpp>
#include <utility>

namespace nbdl {
  namespace detail {
    template <typename Path, typename Fn>
    struct match_path_helper_fn
    {
      Path const& path;
      Fn const& fn;

      template <Store Value>
      constexpr void operator()(Value&& value) const {
        match_path(
          std::forward<Value>(value)
        , hana::drop_front(path)
        , fn
        );
      }

      template <typename Value>
      constexpr void operator()(Value&& value) const {
        fn(std::forward<Value>(value));
      }
    };
  }

  template <typename Store, Path Path, typename ...Fns>
    requires (!EmptyPath<Path>)
  constexpr void match_path_fn::operator()(Store&& s,
                                           Path&& p,
                                           Fns&&... fns) const {
    auto fn = hana::overload_linearly(std::forward<Fns>(fns)...);
    auto helper = detail::match_path_helper_fn<Path, decltype(fn)>{p, std::move(fn)};

    nbdl::match(
      std::forward<Store>(s)
    , hana::front(std::forward<Path>(p))
    , std::move(helper)
    );
  }

  template <Store Store, EmptyPath Path, typename ...Fns>
  constexpr void match_path_fn::operator()(Store&& s,
                                           Path&&,
                                           Fns&&... fns) const {
    auto fn = hana::overload_linearly(std::forward<Fns>(fns)...);
    nbdl::match(
      std::forward<Store>(s)
    , std::move(fn)
    );
  }

  template <typename T, EmptyPath Path, typename ...Fns>
  constexpr void match_path_fn::operator()(T&& value,
                                           Path&&,
                                           Fns&&... fns) const {
    // just yield `value`
    auto fn = hana::overload_linearly(std::forward<Fns>(fns)...);
    fn(std::forward<T>(value));
  }
}

#endif
