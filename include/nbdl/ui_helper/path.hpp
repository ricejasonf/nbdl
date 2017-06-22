//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_UI_HELPER_PATH_HPP
#define NBDL_UI_HELPER_PATH_HPP

#include <nbdl/match_path.hpp>
#include <nbdl/ui_spec.hpp>

#include <boost/hana/type.hpp>
#include <boost/hana/reverse.hpp>
#include <boost/hana/unpack.hpp>
#include <mpdef/list.hpp>

namespace nbdl::ui_helper
{
  namespace hana = boost::hana;

  /*
   * promise_node_match - used to resolve a single node in a path_t spec
   */
  template <typename ...>
  struct path_node_match;

  template <typename T, typename ...P, typename Continuation>
  struct path_node_match<ui_spec::match_type<T, P...>, Continuation>
  {
    Continuation const& cont;

    template <typename Store>
    void operator()(Store const& store) const
    {
      nbdl::match_path(store, mpdef::list<P...>{}, [&](auto const& result)
      {
        if constexpr(decltype(hana::type_c<T> == hana::typeid_(result)){})
          cont(result);
        // else do nothing 
      });
    }
  };

  template <typename ...P, typename Continuation>
  struct path_node_match<ui_spec::get_t<P...>, Continuation>
  {
    Continuation const& cont;

    template <typename Store>
    void operator()(Store const& store) const
    {
      nbdl::match_path(
        store
      , mpdef::list<P...>{} 
      , cont
      );
    }
  };

  template <typename PathNode, typename Continuation>
  inline auto make_path_node_match(Continuation const& cont)
  {
    return path_node_match<PathNode, Continuation>{cont};
  };

  template <typename ...>
  int path_impl;

  template <>
  auto path_impl<> = [](auto const& store, auto const& cont)
  {
    cont(store);
  };

  template <typename P1>
  auto path_impl<P1> = [](auto const& store, auto const& cont)
  {
    path_node_match<P1, decltype(cont)>{cont}(store);
  };

  template <typename P1, typename P2>
  auto path_impl<P1, P2> = [](auto const& store, auto const& cont)
  {
    make_path_node_match<P1>(
    make_path_node_match<P2>(
      cont
    ))(store);
  };

  template <typename P1, typename P2, typename P3, typename ...Ps>
  auto path_impl<P1, P2, P3, Ps...> = [](auto const& store, auto const& cont)
  {
    make_path_node_match<P1>(
    make_path_node_match<P2>(
    make_path_node_match<P3>(
      [&](auto const& result) { path_impl<Ps...>(result, cont); }
    )))(store);
  };

  struct path_fn
  {
    template <typename ...Xs, typename Store, typename Continuation>
    void operator()(ui_spec::path_t<Xs...>, Store const& store, Continuation const& cont) const
    {
      hana::unpack(hana::reverse(mpdef::list<Xs...>{}), [&](auto ...x)
      {
        path_impl<decltype(x)...>(store, cont);
      });
    }
  };

  constexpr path_fn path{};
}

#endif
