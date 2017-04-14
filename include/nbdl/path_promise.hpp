//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_PATH_PROMISE_HPP
#define NBDL_PATH_PROMISE_HPP

#include <nbdl/match_path.hpp>
#include <nbdl/pipe.hpp>
#include <nbdl/promise.hpp>
#include <nbdl/ui_spec.hpp>

#include <boost/hana/type.hpp>
#include <boost/hana/reverse.hpp>
#include <boost/hana/unpack.hpp>
#include <mpdef/list.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  /*
   * promise_match - used to resolve a single node in a path_t spec
   */
  template <typename ...>
  int promise_match;

  template <typename T, typename ...P>
  auto promise_match<ui_spec::match_type<T, P...>> = nbdl::promise([](auto& resolver, auto const& store)
  {
    nbdl::match_path(store, mpdef::list<P...>{}, [&](auto const& result)
    {
      if constexpr(decltype(hana::type_c<T> == hana::typeid_(result)){})
        resolver.resolve(result);
      else
        resolver.reject(result);
    });
  });

  template <typename ...P>
  auto promise_match<ui_spec::get_t<P...>> = nbdl::promise([](auto& resolver, auto const& store)
  {
    nbdl::match_path(store, mpdef::list<P...>{}, [&](auto const& result)
    {
      resolver.resolve(result);
    });
  });

  struct path_promise_fn
  {
    template <typename ...Xs>
    auto operator()(ui_spec::path_t<Xs...>) const
    {
      constexpr auto xs = hana::reverse(mpdef::list<Xs...>{});
      return hana::unpack(xs, [](auto ...x)
      {
        return nbdl::pipe(promise_match<decltype(x)>...);
      });
    }
  };

  constexpr path_promise_fn path_promise{};
}

#endif
