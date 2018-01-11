//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_GET_HPP
#define NBDL_GET_HPP

#include <nbdl/fwd/get.hpp>

#include <nbdl/concept/State.hpp>

#include <boost/hana/at_key.hpp>
#include <boost/hana/concept/searchable.hpp>
#include <boost/hana/index_if.hpp>
#include <boost/hana/equal.hpp>
#include <boost/hana/functional/compose.hpp>
#include <functional>
#include <utility>

namespace nbdl
{
  namespace hana = boost::hana;

  template <typename State, typename Key>
  constexpr decltype(auto) get_fn::operator()(State&& s, Key&& k) const
  {
    using Tag = hana::tag_of_t<State>;
    using Impl = get_impl<Tag>;

    static_assert(
      nbdl::State<State>::value
    , "nbdl::get(store, path) requires 'store' to be State"
    );

    return Impl::apply(std::forward<State>(s), std::forward<Key>(k));
  };

  template <typename State>
  constexpr decltype(auto) get_fn::operator()(State&& s) const
  {
    using Tag = hana::tag_of_t<State>;
    using Impl = get_impl<Tag>;

    static_assert(nbdl::State<State>::value,
      "nbdl::get(store, path) requires 'store' to be State");

    return Impl::apply(std::forward<State>(s));
  };

  template <typename Tag, bool condition>
  struct get_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    static constexpr auto apply(...) = delete;
  };

  template <typename T>
  struct get_impl<std::reference_wrapper<T>, hana::when<nbdl::State<T>::value>>
  {
    template <typename State>
    static constexpr decltype(auto) apply(State s)
    {
      return s.get();
    }

    template <typename State, typename Key>
    static constexpr decltype(auto) apply(State s, Key&& k)
    {
      return nbdl::get(s.get(), std::forward<Key>(k));
    }
  };

  template <typename Tag>
  struct get_impl<Tag, hana::when<hana::Searchable<Tag>::value>>
  {
    template <typename State>
    static constexpr decltype(auto) apply(State&& s)
    {
      return std::forward<State>(s);
    }

    template <typename State, typename Key>
    static constexpr decltype(auto) apply(State&& s, Key&& k)
    {
      if constexpr(hana::Sequence<State>::value)
      {
        using Pred = decltype(hana::compose(hana::equal.to(hana::typeid_(k)), hana::typeid_));
        using Index = decltype(hana::index_if(s, Pred{}).value());
        return hana::at(std::forward<State>(s), Index{});
      }
      else
      {
        return hana::at_key(std::forward<State>(s), std::forward<Key>(k));
      }
    }
  };

  template <typename Tag>
  struct get_impl<Tag, hana::when<hana::Product<Tag>::value and not hana::Searchable<Tag>::value>>
  {
    template <typename State>
    static constexpr decltype(auto) apply(State&& s)
    {
      return std::forward<State>(s);
    }

    template <typename State>
    static constexpr decltype(auto) apply(State&& s, hana::first_t)
    {
      return hana::first(std::forward<State>(s));
    }

    template <typename State>
    static constexpr decltype(auto) apply(State&& s, hana::second_t)
    {
      return hana::second(std::forward<State>(s));
    }
  };

}

#endif
