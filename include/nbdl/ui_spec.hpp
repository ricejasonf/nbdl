//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_UI_SPEC_HPP
#define NBDL_UI_SPEC_HPP

#include <mpdef/list.hpp>
#include <nbdl/promise.hpp>
#include <nbdl/match_path.hpp>

#include <boost/hana/functional/always.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana/type.hpp>

/*
 * nbdl::ui_spec - tools for impls like webui to manipulate paths for Store access
 */
namespace nbdl::ui_spec
{
  namespace hana = boost::hana;

  template <typename ...>
  struct path_t
  { };

  template <typename ...>
  struct get_t
  { };

  template <typename T, typename ...P>
  struct match_type { };

  template <typename T, typename Spec>
  struct when_t { };

  template <typename ...Xs>
  struct make_path
  {
    using type = path_t<get_t<Xs...>>;
  };

  template <typename ...MatchTypeArgs, typename ...Ys, typename ...Xs>
  struct make_path<path_t<match_type<MatchTypeArgs...>, Ys...>, Xs...>
  {
    using type = path_t<get_t<Xs...>, match_type<MatchTypeArgs...>, Ys...>;
  };

  template <typename T, typename Path>
  struct make_path_match_type;

  template <typename T, typename ...P, typename ...Xs>
  struct make_path_match_type<T, path_t<get_t<P...>, Xs...>>
  {
    using type = path_t<match_type<T, P...>, Xs...>;
  };

  template <typename T, typename ...WhenArgs, typename ...P, typename ...Xs>
  struct make_path_match_type<when_t<T, WhenArgs...>, path_t<get_t<P...>, Xs...>>
  {
    using type = path_t<match_type<T, P...>, Xs...>;
  };

  template <typename T, typename ...WhenArgs, typename ...P, typename ...Xs>
  struct make_path_match_type<when_t<T, WhenArgs...>, const path_t<get_t<P...>, Xs...>>
  {
    using type = path_t<match_type<T, P...>, Xs...>;
  };


  /*
   * get
   */

  struct get_fn
  {
    template <typename ...Xs>
    constexpr auto operator()(Xs...) const
      -> typename make_path<Xs...>::type
    { return {}; }
  };

  constexpr get_fn get{};

  /*
   * when<T>
   */

  namespace detail
  {
    struct when_lift_spec_fn
    {
      template <typename T, typename Spec, typename Path>
      constexpr auto operator()(when_t<T, Spec>, Path) const
      {
        using MatchedPath = typename make_path_match_type<T, Path>::type;
        if constexpr(std::__invokable<Spec, MatchedPath>::value)
        {
          return when_t<T, Spec>{};
        }
        else
        {
          return when_t<T, decltype(hana::always(Spec{}))>{};
        }
      }
    };
  }

  template <typename T = void>
  struct when_fn
  {
    template <typename Spec>
    constexpr auto operator()(Spec) const
      -> when_t<T, Spec>
    { return {}; }
  };

  template <typename T = void>
  constexpr when_fn<T> when{};

  /*
   * match
   */

  template <typename ...> 
  struct match_t { };

  struct match_fn
  {
    template <typename ...Xs, typename ...Whens>
    constexpr auto operator()(path_t<Xs...>, Whens...) const
      -> match_t<
        path_t<Xs...>
      , decltype(detail::when_lift_spec_fn{}(std::declval<Whens>(), path_t<Xs...>{}))...
      >
    { return {}; }
  };

  constexpr match_fn match{};

  /*
   * promise_match - used to resolve a single node in a path_t spec
   */

  template <typename ...>
  int promise_match;

  template <typename T, typename ...P>
  auto promise_match<match_type<T, P...>> = nbdl::promise([](auto& resolver, auto const& store)
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
  auto promise_match<get_t<P...>> = nbdl::promise([](auto& resolver, auto const& store)
  {
    nbdl::match_path(store, mpdef::list<P...>{}, [&](auto const& result)
    {
      resolver.resolve(result);
    });
  });
}

#endif
