//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_UI_SPEC_HPP
#define NBDL_UI_SPEC_HPP

#include <mpdef/MPDEF_DIRECTIVE.hpp>
#include <mpdef/list.hpp>
#include <nbdl/get_path.hpp>
#include <nbdl/match_path.hpp>
#include <nbdl/pipe.hpp>
#include <nbdl/promise.hpp>

#include <boost/hana/core/tag_of.hpp>
#include <boost/hana/bool.hpp>
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

  MPDEF_DIRECTIVE_LIST(concat);

  struct noop_t { }; // used implicitly in otherwise() et al

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

  template <typename ...GetArgs, typename ...Ys, typename ...Xs>
  struct make_path<path_t<get_t<GetArgs...>, Ys...>, Xs...>
  {
    using type = path_t<get_t<GetArgs..., Xs...>, Ys...>;
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

    struct make_cond_pair_fn
    {
      template <typename T, typename Spec, typename Path>
      constexpr auto operator()(when_t<T, Spec>, Path) const
      {
        if constexpr(std::__invokable<Spec, Path>::value)
        {
          return mpdef::pair<T, decltype(std::declval<Spec>()(Path{}))>{};
        }
        else
        {
          return mpdef::pair<T, Spec>{};
        }
      }
    };
  }

  /*
   * when<T> - for matching types
   */

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
   * cond - alias for matching with predicate
   *        (to be used with predicates)
   */

  struct cond_fn
  {
    template <typename Pred, typename Spec>
    constexpr auto operator()(Pred pred, Spec) const
      -> when_t<decltype(pred), Spec>
    { return {}; }
  };

  constexpr cond_fn cond{};

  /*
   * otherwise - alias for default branch
   */

  struct otherwise_fn
  {
    constexpr auto operator()() const
      -> when_t<decltype(hana::always(hana::true_c)), noop_t>
    { return {}; }

    template <typename Spec>
    constexpr auto operator()(Spec) const
      -> when_t<decltype(hana::always(hana::true_c)), Spec>
    { return {}; }
  };

  constexpr otherwise_fn otherwise{};

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

  /*
   * match_if (uses predicates instead of types)
   */

  struct match_if_tag { };

  template <typename ...> 
  struct match_if_t { };

  struct match_if_fn
  {
    template <typename ...Xs, typename ...Whens>
    constexpr auto operator()(path_t<Xs...>, Whens...) const
      -> match_if_t<
        path_t<Xs...>
      , mpdef::list<
          decltype(detail::make_cond_pair_fn{}(std::declval<Whens>(), path_t<Xs...>{}))...
        >
      >
    { return {}; }
  };

  constexpr match_if_fn match_if{};

  /*
   * for_each - for_each(path, fn(iterator_path))
   */

  struct for_each_tag { };

  template <typename ...> 
  struct for_each_t { };

  struct for_each_fn
  {
    template <typename ...Xs, typename SpecFn>
    constexpr auto operator()(path_t<Xs...>, SpecFn) const
      -> for_each_t<
        path_t<Xs...>
      , SpecFn 
      >
    { return {}; }
  };

  constexpr for_each_fn for_each{};

  /*
   * equal
   */

  template <typename X>
  struct equal_
  {
    template <typename Y>
    constexpr auto operator()(Y const& y) const
    {
      if constexpr(hana::is_a<hana::string_tag, X>)
      {
        return hana::equal(hana::to<char const*>(X{}), y);
      }
      else
      {
        return hana::equal(X{}, y);
      }
    }
  };

  struct equal_fn
  {
    template <typename X>
    constexpr auto operator()(X) const
      -> equal_<X>
    { return {}; }
  };

  constexpr equal_fn equal{};

  namespace detail
  {
    template <typename ...>
    struct get_type_at_path_impl { };

    template <typename T, typename ...Xs>
    struct get_type_at_path_impl<T, path_t<get_t<Xs...>>>
    {
      // arg what is T in this context
      using type = decltype(nbdl::get_path(std::declval<T>(), hana::make_tuple(std::declval<Xs>()...)));
    };

    template <typename Store, typename T, typename ...MatchTypeArgs, typename ...Xs, typename ...Ys>
    struct get_type_at_path_impl<Store, path_t<get_t<Xs...>, match_type<T, MatchTypeArgs...>, Ys...>>
    {
      using type = decltype(nbdl::get_path(std::declval<T>(), hana::make_tuple(std::declval<Xs>()...)));
    };

    template <typename Store, typename PathSpec>
    using get_type_at_path = typename get_type_at_path_impl<Store, PathSpec>::type;
  }
}

namespace boost::hana
{
  template <typename ...T>
  struct tag_of<nbdl::ui_spec::match_t<T...>>
  {
    using type = nbdl::ui_spec::match_tag;
  };

  template <typename ...T>
  struct tag_of<nbdl::ui_spec::match_if_t<T...>>
  {
    using type = nbdl::ui_spec::match_if_tag;
  };

  template <typename ...T>
  struct tag_of<nbdl::ui_spec::path_t<T...>>
  {
    using type = nbdl::ui_spec::path_tag;
  };

  template <typename ...T>
  struct tag_of<nbdl::ui_spec::for_each_t<T...>>
  {
    using type = nbdl::ui_spec::for_each_tag;
  };
}

#endif
