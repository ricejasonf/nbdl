//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_UI_SPEC_HPP
#define NBDL_UI_SPEC_HPP

#include <mpdef/list.hpp>
#include <nbdl/fwd/ui_spec.hpp>
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
  template <typename T>
  constexpr auto type_inst(T) {
    return typename T::type{};
  }

  // make_type implemented here because we don't want
  // idempotence
  template <typename T>
  constexpr auto make_type(T) {
    return hana::type<T>{};
  }

  namespace hana = boost::hana;

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

  template <typename T, typename PathSpec>
  struct make_path_match_type;

  template <typename T, typename ...P, typename ...Xs>
  struct make_path_match_type<T, path_t<get_t<P...>, Xs...>>
  {
    using type = path_t<match_type<T, P...>, Xs...>;
  };

  /*
   * apply(fn)
   */

  struct apply_tag { };

  template <typename Fn, typename ParamsSpec>
  struct apply_t { };

  template <typename Fn>
  struct apply_fn_fn
  {
    template <typename ...Params>
    constexpr auto operator()(Params...) const
      -> apply_t<Fn, mpdef::list<Params...>>
    { return {}; }
  };

  struct apply_fn
  {
    template <typename Fn>
    constexpr auto operator()(Fn) const
      -> apply_fn_fn<Fn>
    { return {}; }
  };

  constexpr apply_fn apply{};

  /*
   * get
   */

  heavy_macro get(...x) = type_inst(type_inst(hana::template_<make_path>(
        make_type(mpdef::to_constant(x))...)));

  struct get_fn {
    auto operator()(auto ...x)
    {
      return get(decltype(x){}...);
    }
  };

  /*
   * match_when<T>
   */

  template <typename T>
  struct match_when_fn
  {
    template <typename PathSpec>
    constexpr auto operator()(PathSpec) const
      -> typename make_path_match_type<T, PathSpec>::type
    { return {}; }
  };

  template <typename T>
  constexpr match_when_fn<T> match_when{};

  /*
   * key_at - Use key from value at location in store
   */
  struct key_at_fn
  {
    template <typename PathSpec>
    constexpr auto operator()(PathSpec) const
      -> key_at_t<PathSpec>
    { return {}; }
  };

  constexpr key_at_fn key_at{};

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
      template <typename Pred, typename Spec, typename Path>
      constexpr auto operator()(when_t<Pred, Spec>, Path) const
      {
        if constexpr(std::__invokable<Spec, Path>::value)
        {
          return mpdef::pair<Pred, decltype(std::declval<Spec>()(Path{}))>{};
        }
        else
        {
          return mpdef::pair<Pred, Spec>{};
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

    constexpr auto operator()() const
      -> when_t<T, noop_t>
    { return {}; }
  };

  template <typename T = void>
  constexpr when_fn<T> when{};

  /*
   * cond - alias for matching with predicate
   *        (to be used with predicates)
   */

  heavy_macro cond(pred, spec) = type_inst(hana::template_<when_t>(
      make_type(pred), make_type(mpdef::to_constant(spec))));

  /*
   * otherwise - alias for default branch
   */

  heavy_macro otherwise(spec) = type_inst(hana::template_<when_t>(
      make_type(hana::always(hana::true_c)),
      make_type(mpdef::to_constant(spec))));

  /*
   * branch_spec
   */

  struct branch_spec_tag;

  template <typename Tag, typename PathSpec, typename Branches>
  struct branch_spec { };

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
      -> branch_spec<
        match_tag
      , path_t<Xs...>
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
      -> branch_spec<
        match_if_tag
      , path_t<Xs...>
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

  heavy_macro equal(x) = type_inst(hana::template_<equal_>(
        make_type(mpdef::to_constant(x))));

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
  template <typename Fn, typename ParamsSpec>
  struct tag_of<nbdl::ui_spec::apply_t<Fn, ParamsSpec>>
  {
    using type = nbdl::ui_spec::apply_tag;
  };

  template <typename ParamsSpec>
  struct tag_of<nbdl::ui_spec::key_at_t<ParamsSpec>>
  {
    using type = nbdl::ui_spec::key_at_tag;
  };

  template <typename Tag, typename PathSpec, typename Branches>
  struct tag_of<nbdl::ui_spec::branch_spec<Tag, PathSpec, Branches>>
  {
    using type = nbdl::ui_spec::branch_spec_tag;
  };

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
