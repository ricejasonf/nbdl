//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_STORE_COMPOSE_HPP
#define NBDL_FWD_STORE_COMPOSE_HPP

namespace nbdl
{
  namespace hana = boost::hana;

  // store_compose - Adds a value to a store via composition.
  //                 The value is accessed via the provided key.
  //                 For actions ont the value, the action must
  //                 be wrapped with store_composite_action(key, action)
  //                 The original store is still accessed the same way.
  struct store_compose_fn
  {
    template <typename Key, typename Value, typename Store>
    constexpr auto operator()(Key, Value&&, Store&&) const;
  };

  constexpr store_compose_fn store_compose{};

  // Creates wrapper used to dispatch action
  // to the added value in a store composite
  // when the key matches.
  struct store_composite_action_fn
  {
    template <typename Key, typename Action>
    constexpr auto operator()(Key, Action&&) const;
  };

  constexpr store_composite_action_fn store_composite_action{};
}

#endif
