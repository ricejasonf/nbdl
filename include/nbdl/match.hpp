//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MATCH_HPP
#define NBDL_MATCH_HPP

#include <nbdl/concept/Store.hpp>
#include <nbdl/fwd/match.hpp>
#include <nbdl/get.hpp>
#include <nbdl/variant.hpp>

#include <boost/hana/bool.hpp>
#include <boost/hana/core/default.hpp>
#include <boost/hana/core/tag_of.hpp>
#include <boost/hana/core/when.hpp>
#include <boost/hana/type.hpp>
#include <functional>
#include <type_traits>
#include <utility>

namespace nbdl
{
  namespace detail
  {
    template <typename Impl, typename Store, typename Fn, typename = void>
    struct matches_identity : std::false_type { };

    template <typename Impl, typename Store, typename Fn>
    struct matches_identity<Impl, Store, Fn,
      decltype(Impl::apply(std::declval<Store>(), std::declval<Fn>()))>
      : std::true_type
    { };
  }

  template<typename Store, typename Key, typename Fn>
  constexpr void match_fn::operator()(Store&& s, Key&& k, Fn&& fn) const
  {
    using Tag = hana::tag_of_t<Store>;
    using Impl = match_impl<Tag>;

    static_assert(
      nbdl::Store<Store>::value
    , "nbdl::match(store, key, fn) requires 'store' to be a Store"
    );

    Impl::apply(
      std::forward<Store>(s)
    , std::forward<Key>(k)
    , std::forward<Fn>(fn)
    );
  };

  template<typename Store, typename Fn>
  constexpr void match_fn::operator()(Store&& s, Fn&& fn) const
  {
    using Tag = hana::tag_of_t<Store>;
    using Impl = match_impl<Tag>;

    static_assert(
      nbdl::Store<Store>::value
    , "nbdl::match(store, fn) requires 'store' to be a Store"
    );

    if constexpr(detail::matches_identity<Impl, Store, Fn>::value)
    {
      Impl::apply(
        std::forward<Store>(s)
      , std::forward<Fn>(fn)
      );
    }
    else
    {
      std::forward<Fn>(fn)(
        std::forward<Store>(s)
      );
    }
  };

  template<typename Tag, bool condition>
  struct match_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    static constexpr auto apply(...) = delete;
  };

  template<typename Tag>
  struct match_impl<Tag, hana::when<nbdl::State<Tag>::value>>
  {
    template <typename Store, typename Fn>
    static constexpr void apply(Store&& s, Fn&& fn)
    {
      std::forward<Fn>(fn)(
        nbdl::get(std::forward<Store>(s))
      );
    }

    template <typename Store, typename Key, typename Fn>
    static constexpr void apply(Store&& s, Key&& k, Fn&& fn)
    {
      std::forward<Fn>(fn)(
        nbdl::get(std::forward<Store>(s), std::forward<Key>(k))
      );
    }
  };

  template<typename T>
  struct match_impl<std::reference_wrapper<T>
                  , hana::when<(nbdl::Store<T>::value and not nbdl::State<T>::value)>>
  {
    template <typename Store, typename ...Args>
    static constexpr void apply(Store s, Args&& ...args)
    {
      nbdl::match(s.get(), std::forward<Args>(args)...);
    }
  };

  //
  // Visitors
  //

  template <typename T>
  template <typename F>
  constexpr auto match_when_fn<T>::operator()(F&& f) const
  { return match_when_t<T, std::decay_t<F>>{std::forward<F>(f)}; }

  template <typename T, typename F>
  template <typename X>
  constexpr void match_when_t<T, F>::operator()(X&& x) const
  {
    if constexpr(std::is_same<T, std::decay_t<X>>::value)
    {
      f(std::forward<X>(x));
    }
  }

  template <typename Searchable, typename Otherwise>
  constexpr auto mapped_overload_fn::operator()(Searchable&& s, Otherwise&& o) const
  {
    return mapped_overload_t<std::decay_t<Searchable>, std::decay_t<Otherwise>>{
      std::forward<Searchable>(s), std::forward<Otherwise>(o)
    };
  }

  template <typename Searchable, typename Otherwise>
  template <typename X>
  constexpr void mapped_overload_t<Searchable, Otherwise>::operator()(X&& x) const
  { hana::find(map, hana::typeid_(x)).value_or(otherwise)(std::forward<X>(x)); }
}

#endif
