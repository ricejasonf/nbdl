//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_UI_HELPER_MATCH_PATH_SPEC_HPP
#define NBDL_UI_HELPER_MATCH_PATH_SPEC_HPP

#include <nbdl/fwd/ui_helper/match_path_spec.hpp>
#include <nbdl/ui_helper/match_params_spec.hpp>
#include <nbdl/fwd/ui_spec.hpp>
#include <nbdl/match_path.hpp>
#include <nbdl/ui_spec.hpp>

#include <boost/hana/basic_tuple.hpp>
#include <boost/hana/core/tag_of.hpp>
#include <boost/hana/type.hpp>
#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/list.hpp>
#include <mpdef/list.hpp>

namespace nbdl::ui_helper::detail
{
  namespace hana = boost::hana;
  using namespace boost::mp11;

  template <typename P>
  using contains_key_at_pred = std::is_same<ui_spec::key_at_tag, hana::tag_of_t<P>>;

  template <typename ...P>
  using contains_key_at = mp_count_if<mp_list<P...>, contains_key_at_pred>;

  /*
   * promise_node_match - used to resolve a single node in a path_t spec
   */
  template <typename ...>
  struct path_node_match;

  template <typename T, typename ...P, typename Fn>
  struct path_node_match<ui_spec::match_type<T, P...>, Fn>
  {
    Fn const& fn;

    using operator()(auto self, auto store)
    {
      // use params as path
      match_params_spec(store, mpdef::list<P...>{}, [&](auto const& ...x)
      {
        nbdl::match_path(
          store
        , hana::make_basic_tuple(x...)
        , [&](auto const& result)
          {
            if constexpr(decltype(hana::type_c<T> == hana::typeid_(result)){})
              self.fn(result);
            // else do nothing 
          }
        );
      });
    }
  };

  template <typename ...P, typename Fn>
  struct path_node_match<ui_spec::get_t<P...>, Fn>
  {
    Fn const& fn;

    using operator()(auto self, auto store)
    {
      // use params as path
      match_params_spec(store, mpdef::list<P...>{}, [&](auto const& ...x)
      {
        nbdl::match_path(
          store
        , hana::make_basic_tuple(x...)
        , self.fn
        );
      });
    }
  };

  template <typename PathNode, typename Fn>
  inline auto make_path_node_match(Fn const& fn)
  {
    return path_node_match<PathNode, Fn>{fn};
  };

  template <typename ...>
  struct path_impl;

  template <>
  struct path_impl<>
  {
    static using apply(auto store, auto fn) {
      fn(store);
    }
  };

  template <typename P1>
  struct path_impl<P1>
  {
    static using apply(auto store, auto fn) {
      path_node_match<P1, decltype(fn)>{fn}(store);
    }
  };

  template <typename P1, typename P2>
  struct path_impl<P1, P2>
  {
    static using apply(auto store, auto fn)
    {
      make_path_node_match<P1>(
      make_path_node_match<P2>(
        fn
      ))(store);
    }
  };

  template <typename P1, typename P2, typename P3, typename ...Ps>
  struct path_impl<P1, P2, P3, Ps...>
  {
    static using apply(auto store, auto fn)
    {
      make_path_node_match<P1>(
      make_path_node_match<P2>(
      make_path_node_match<P3>(
        [&](auto const& result) { path_impl<Ps...>::apply(result, fn); }
      )))(store);
    }
  };
}

namespace nbdl::ui_helper
{
  using namespace boost::mp11;

  template <typename Store, typename PathSpec, typename Fn>
  void match_path_spec_fn::operator()(Store const& store, PathSpec, Fn&& fn) const
  {
    mp_apply<detail::path_impl, mp_reverse<PathSpec>>::apply(
      store
    , std::forward<Fn>(fn)
    );
  }

  template <typename T, typename PathSpec>
  template <typename Store>
  bool match_path_spec_predicate_fn<T, PathSpec>::operator()(Store const& store) const
  {
    bool result = false;
    using Path = mp_push_front<PathSpec, ui_spec::match_type<T>>;

    match_path_spec(store, Path{}, [&](auto&&) { result = true; });

    return result;
  }
}

#endif
