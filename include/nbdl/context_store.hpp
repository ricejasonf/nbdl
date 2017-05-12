//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONTEXT_STORE_HPP
#define NBDL_CONTEXT_STORE_HPP

#include <nbdl/apply_action.hpp>
#include <nbdl/apply_foreign_action.hpp>
#include <nbdl/make_store.hpp>
#include <nbdl/message.hpp>

#include <type_traits>

namespace nbdl
{
  // Wrapper for stores used within a nbdl::context
  template <typename Store>
  struct context_store { };

  template <typename Store>
  struct context_store_t
  { 
    using hana_tag = context_store<Store>;

    Store store;

    context_store_t()
      : store()
    { }
  };

  template <typename Store>
  struct make_store_impl<context_store<Store>>
  {
    static constexpr auto apply(...)
      -> context_store_t<Store>
    { return {}; }
  };

  template <typename S>
  struct apply_action_impl<context_store<S>>
  {
    template <typename Store, typename Message>
    static constexpr auto apply(Store&& s, Message&& m)
    {
      if constexpr(
          message::is_downstream<Message>
      && (message::is_update<Message> || message::is_update_raw<Message>)
      )
      {
        return nbdl::apply_action(
          std::forward<Store>(s).store
        , message::get_payload(std::forward<Message>(m))
        );
      }
      else
      {
        return hana::false_c;
      }
    }
  };

  template <typename S>
  struct apply_foreign_action_impl<context_store<S>>
  {
    template<typename Store, typename Message, typename Fn>
    static constexpr void apply(Store&& s, Message&& m, Fn&& fn)
    {
      // just proxy to the contained store
      nbdl::apply_foreign_action(
        std::forward<Store>(s).store
      , std::forward<Message>(m)
      , std::forward<Fn>(fn)
      );
    }
  };

  template <typename S>
  struct match_impl<context_store<S>>
  {
    template <typename Store, typename Key, typename Fn>
    static constexpr void apply(Store&& s, Key&&, Fn&& fn)
    {
      // just proxy to the contained store
      std::forward<Fn>(fn)(std::forward<Store>(s).store);
    }
  };

} // nbdl

#endif