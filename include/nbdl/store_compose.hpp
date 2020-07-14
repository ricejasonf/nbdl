//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_STORE_COMPOSE_HPP
#define NBDL_STORE_COMPOSE_HPP

#include <nbdl/apply_action.hpp>
#include <nbdl/concept/Store.hpp>
#include <nbdl/fwd/store_compose.hpp>
#include <nbdl/match.hpp>

#include <boost/hana/core/is_a.hpp>
#include <type_traits>
#include <utility>

namespace nbdl
{
  namespace hana = boost::hana;

  namespace detail
  {
    template <typename Key>
    struct store_composite_tag { };

    template <typename Key, typename Left, typename Right>
    struct store_composite_t
    {
      using hana_tag = store_composite_tag<Key>;

      Left left;
      Right right;
    };

    template <typename Key>
    struct store_composite_action_tag { };

    template <typename Key, typename Action>
    struct store_composite_action_t
    {
      using hana_tag = store_composite_action_tag<Key>;

      Action action;
    };
  }

  template <typename Key, typename Value, typename Store>
  constexpr auto store_compose_fn::operator()(Key, Value&& v, Store&& s) const
  {
    using Left = std::decay_t<Store>;
    using Right = std::decay_t<Value>;

    return detail::store_composite_t<Key, Left, Right>{
      std::forward<Store>(s)
    , std::forward<Value>(v)
    };
  }

  template <typename Key, typename Action>
  constexpr auto store_composite_action_fn::operator()(Key, Action&& a) const
  {
    return detail::store_composite_action_t<Key, std::decay_t<Action>>{
      std::forward<Action>(a)
    };
  }

  // apply_action_impl
  template <typename Key>
  struct apply_action_impl<detail::store_composite_tag<Key>>
  {
    template <typename Store, typename Action>
    static constexpr auto apply(Store& s, Action&& a)
    {
      if constexpr(hana::is_a<detail::store_composite_action_tag<Key>, Action>)
      {
        return nbdl::apply_action(s.right, std::forward<Action>(a).action);
      }
      else
      {
        return nbdl::apply_action(s.left, std::forward<Action>(a));
      }
    }
  };

  // match_impl
  template <typename Key>
  struct match_impl<detail::store_composite_tag<Key>>
  {
    template <typename Store, typename KeyArg, typename Fn>
    static constexpr void apply(Store& s, KeyArg&& k, Fn&& fn)
    {
      if constexpr(std::is_same<Key, std::decay_t<KeyArg>>::value)
      {
        if constexpr(nbdl::Store<decltype(s.right)>)
        {
          nbdl::match(s.right, std::forward<Fn>(fn));
        }
        else
        {
          std::forward<Fn>(fn)(s.right);
        }
      }
      else
      {
        nbdl::match(s.left, std::forward<KeyArg>(k), std::forward<Fn>(fn));
      }
    }
  };
}

#endif
