//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_UI_SPEC_HPP
#define NBDL_UI_SPEC_HPP

#include <mpdef/list.hpp>
#include <nbdl/match_path.hpp>
#include <nbdl/pipe.hpp>
#include <nbdl/promise.hpp>

#include <boost/hana/core/tag_of.hpp>
#include <boost/hana/functional/always.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana/type.hpp>
#include <boost/hana/unpack.hpp>
#include <mpdef/pair.hpp>

/*
 * nbdl::ui_spec - tools for impls like webui to manipulate paths for Store access
 */
namespace nbdl::ui_spec
{
  namespace hana = boost::hana;

  struct path_tag { };

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
    struct make_when_pair_fn
    {
      template <typename T, typename Spec, typename Path>
      constexpr auto operator()(when_t<T, Spec>, Path) const
      {
        using MatchedPath = typename make_path_match_type<T, Path>::type;
        if constexpr(std::__invokable<Spec, MatchedPath>::value)
        {
          return mpdef::pair<T, decltype(std::declval<Spec>()(MatchedPath{}))>{};
        }
        else
        {
          return mpdef::pair<T, Spec>{};
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

  struct match_tag { };

  template <typename ...> 
  struct match_t { };

  struct match_fn
  {
    template <typename ...Xs, typename ...Whens>
    constexpr auto operator()(path_t<Xs...>, Whens...) const
      -> match_t<
        path_t<Xs...>
      , mpdef::list<
          decltype(detail::make_when_pair_fn{}(std::declval<Whens>(), path_t<Xs...>{}))...
        >
      >
    { return {}; }
  };

  constexpr match_fn match{};

#if 0 //not used yet
  /*
   * bind 
   *  - Takes a function and parameters
   *    which can be path_specs
   *  - not sure about the name here
   */

  template <typename Fn, typename ...Params>
  struct bind_impl<Fn, Params...>
  {
    template <typename Resolver, typename Element>
    void operator()(Resolver& resolve, Element&& el) const
    {
      Fn{}(el, Params{}...);
      resolve(std::forward<Element>(el));
    }
  };

  template <typename Store, typename Fn, typename ...Params>
  struct mut_bind_impl<Store, Fn, Params...>
  {
    Store const& store;
    Fn fn;

    mut_bind_impl(Store const& s)
      : store(s)
      , fn()
    { }

    template <typename Resolver, typename Element>
    void operator()(Resolver& resolve, Element&& el)
    {
      nbdl::run_sync(
        nbdl::pipe(
          nbdl::params_promise(Params{})
        , hana::fuse([](auto const& ...params) { fn(el, params...); })
        )
      , store
      );
      resolve(std::forward<Element>(el));
    }

    void update()
    {
      nbdl::run_sync(
        nbdl::pipe(
          nbdl::params_promise(Params{})
        , hana::fuse([](auto const& ...params) { fn.update(params...); })
        )
      , store
      );
    }
  };

  struct bind_fn
  {
    template <typename Fn, typename ...Params>
    constexpr auto operator()(Fn, Params...) const
    {
      return bind_impl<Fn, Params...>{};
    }
  };

  constexpr bind_fn bind{};
#endif
}

namespace boost::hana
{
  template <typename ...T>
  struct tag_of<nbdl::ui_spec::match_t<T...>>
  {
    using type = nbdl::ui_spec::match_tag;
  };

  template <typename ...T>
  struct tag_of<nbdl::ui_spec::path_t<T...>>
  {
    using type = nbdl::ui_spec::path_tag;
  };
}

#endif
