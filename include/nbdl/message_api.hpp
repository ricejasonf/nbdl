//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MESSAGE_API_HPP
#define NBDL_MESSAGE_API_HPP

#include <mpdef/Map.hpp>
#include <mpdef/Pair.hpp>
#include <nbdl/message.hpp>
#include <Uid.hpp> // FIXME

#include <boost/hana/type.hpp>
#include <boost/hana/traits.hpp>
#include <utility>

namespace nbdl
{
  namespace hana = boost::hana;

  namespace detail
  {
    template <typename Channel, typename Action, typename Path>
    struct message_lookup_predicate_fn
    {
      template <typename MessageType>
      constexpr auto operator()(MessageType const&)
      {
        using hana::traits::decay;
        using M = typename MessageType::type;
        return
              decay(hana::type_c<decltype(message::get_channel(std::declval<M>()))>)
                == decay(hana::type_c<Channel>)
          &&  decay(hana::type_c<decltype(message::get_action(std::declval<M>()))>)
                == decay(hana::type_c<Action>)
          &&  decay(hana::type_c<decltype(message::get_path(std::declval<M>()))>)
                == decay(hana::type_c<Path>)
          ;
      }
    };
  }

  // wrapper for safe overloading
  struct from_root
  {
    bool is_from_root;
  };

  template <typename upstreamTypes, typename downstreamTypes>
  struct message_api
  {
    using message::channel::upstream;
    using message::channel::downstream;
    using message::action::create;
    using message::action::read;
    using message::action::update;
    using message::action::update_raw;
    using message::action::delete_;

    static constexpr auto type_list_by_channel = mpdef::make_map(
      mpdef::make_pair(hana::type_c<upstream>, hana::type_c<upstreamTypes>),
      mpdef::make_pair(hana::type_c<downstream>, hana::type_c<DownstreamTypes>)
    );

    template <typename Channel, typename Action, typename Path>
    constexpr auto get_message_type(Channel, Action, Path) const
    {
      using Messages = typename decltype(
        hana::at_key(type_list_by_channel, hana::type_c<Channel>)
      )::type;
      constexpr auto message_type = hana::find_if(Messages{},
        detail::message_lookup_predicate_fn<Channel, Action, Path>{});

      static_assert(decltype(hana::is_just(message_type))::value
        , "This message API does not support the channel/action for this path.");

      return *message_type;
    }

    template <typename Channel, typename Action, typename Path, typename ...T>
    auto make_message(Channel const& channel, Action const& action, Path&& path, T&& ...t) const
    {
      using Message = typename decltype(get_message_type(channel, action, path))::type;
      // temporary hack
      return hana::if_(hana::traits::decay(hana::type_c<Channel>) == hana::type_c<upstream>,
        Message(
          channel,
          action,
          std::forward<Path>(path),
          nbdl::uid{},
          std::forward<T>(t)... // <== is_from_root, payload and private payload
        )
      , Message(
          channel,
          action,
          std::forward<Path>(path),
          nbdl::uid{},
          hana::optional<>, // would be is_from_root (should get rid of this)
          std::forward<T>(t)... // <== payload and private payload
        )
    }

    // Create
    template <typename ...T>
    decltype(auto) make_upstream_create_message(T&& ...t) const
    {
      return make_message(upstream{}, create{}, std::forward<T>(t)...);
    }

    template <typename ...T>
    decltype(auto) make_downstream_create_message(T&& ...t) const
    {
      return make_message(downstream{}, create{}, false, std::forward<T>(t)...);
    }

    template <typename ...T>
    decltype(auto) make_downstream_create_message(nbdl::from_root r, T&& ...t) const
    {
      return make_message(downstream{}, create{}, r.is_from_root, std::forward<T>(t)...);
    }

    // Read
    template <typename ...T>
    decltype(auto) make_upstream_read_message(T&& ...t) const
    {
      return make_message(upstream{}, read{}, std::forward<T>(t)...);
    }

    template <typename ...T>
    decltype(auto) make_downstream_read_message(T&& ...t) const
    {
      return make_message(downstream{}, read{}, false, std::forward<T>(t)...);
    }

    template <typename ...T>
    decltype(auto) make_downstream_read_message(nbdl::from_root r, T&& ...t)
    {
      return make_message(downstream{}, read{}, r.is_from_root, std::forward<T>(t)...);
    }

    // Update
    template <typename ...T>
    decltype(auto) make_upstream_update_message(T&& ...t) const
    {
      return make_message(upstream{}, update{}, std::forward<T>(t)...);
    }

    template <typename ...T>
    decltype(auto) make_downstream_update_message(T&& ...t) const
    {
      return make_message(downstream{}, update{}, false, std::forward<T>(t)...);
    }

    template <typename ...T>
    decltype(auto) make_downstream_update_message(nbdl::from_root r, T&& ...t) const
    {
      return make_message(downstream{}, update{}, r.is_from_root, std::forward<T>(t)...);
    }

    // UpdateRaw
    template <typename ...T>
    decltype(auto) make_upstream_update_raw_message(T&& ...t) const
    {
      return make_message(upstream{}, update_raw{}, std::forward<T>(t)...);
    }

    template <typename ...T>
    decltype(auto) make_downstream_update_raw_message(T&& ...t) const
    {
      return make_message(downstream{}, update_raw{}, false, std::forward<T>(t)...);
    }

    template <typename ...T>
    decltype(auto) make_downstream_update_raw_message(nbdl::from_root r, T&& ...t) const
    {
      return make_message(downstream{}, update_raw{}, r.is_from_root, std::forward<T>(t)...);
    }

    // Delete
    template <typename ...T>
    decltype(auto) make_upstream_delete_message(T&& ...t) const
    {
      return make_message(upstream{}, delete_{}, std::forward<T>(t)...);
    }

    template <typename ...T>
    decltype(auto) make_downstream_delete_message(T&& ...t) const
    {
      return make_message(downstream{}, delete_{}, false, std::forward<T>(t)...);
    }

    template <typename ...T>
    decltype(auto) make_downstream_delete_message(nbdl::from_root r, T&& ...t) const
    {
      return make_message(downstream{}, delete_{}, r.is_from_root, std::forward<T>(t)...);
    }
  };
} // nbdl

#endif
