//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_TEST_NETWORK_STORE_HPP
#define NBDL_TEST_NETWORK_STORE_HPP

#include <nbdl/apply_message.hpp>
#include <nbdl/match.hpp>
#include <nbdl/message.hpp>

#include <type_traits>

namespace nbdl_test
{
  // Lifts regular stores to receive update messages for testing
  template <typename Store>
  struct network_store { };

  template <typename Store>
  struct network_store_t
  { 
    using hana_tag = network_store<Store>;

    Store store;
  };

  struct make_network_store_fn
  {
    template <typename Store>
    constexpr auto operator()(Store&& s) const
    {
      return network_store_t<Store>{
        std::forward<Store>(s)
      };
    }
  };

  constexpr make_network_store_fn make_network_store{};
}

namespace nbdl
{
  template <typename S>
  struct apply_message_impl<nbdl_test::network_store<S>>
  {
    template <typename Store, typename Message>
    static constexpr auto apply(Store& s, Message&& m)
    {
      static_assert(message::is_update<Message>);
      nbdl::match_path(s, message::get_path(m), [&](auto& value)
      {
        value = message::get_payload(std::forward<Message>(m));
      });
      return true;
    }
  };

  template <typename S>
  struct match_impl<nbdl_test::network_store<S>>
  {
    template <typename Store, typename Fn>
    static constexpr void apply(Store&& s, Fn&& fn)
    {
      std::forward<Fn>(fn)(std::forward<Store>(s).store);
    }

    template <typename Store, typename Key, typename Fn>
    static constexpr void apply(Store&& s, Key&& k, Fn&& fn)
    {
      nbdl::match(
        std::forward<Store>(s).store
      , std::forward<Key>(k)
      , std::forward<Fn>(fn)
      );
    }
  };
}

#endif
