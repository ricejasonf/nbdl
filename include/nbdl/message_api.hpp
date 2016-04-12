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
#include <nbdl/variant.hpp>

#include <boost/hana/type.hpp>
#include <boost/hana/traits.hpp>
#include <functional>
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

    template <typename Message, typename = void>
    struct make_is_from_root_fn;

    template <typename Message>
    struct make_is_from_root_fn<Message,
      std::enable_if_t<decltype(
        hana::is_just(message::get_maybe_is_from_root(std::declval<Message>()))
      )::value>
    >
    {
      template <typename IsFromRoot>
      constexpr auto operator()(IsFromRoot)
      {
        return hana::just(IsFromRoot::value);
      }
    };

    template <typename Message>
    struct make_is_from_root_fn<Message,
      std::enable_if_t<decltype(
        !hana::is_just(message::get_maybe_is_from_root(std::declval<Message>()))
      )::value>
    >
    {
      constexpr auto operator()(...)
      {
        return hana::nothing;
      }
    };
  } // detail

  // wrapper for safe overloading
  struct from_root
  {
    bool is_from_root;
  };

  template <typename UpstreamTypes, typename DownstreamTypes>
  class message_api
  {
    using upstream    = message::channel::upstream;
    using downstream  = message::channel::downstream;
    using create      = message::action::create;
    using read        = message::action::read;
    using update      = message::action::update;
    using update_raw  = message::action::update_raw;
    using delete_     = message::action::delete_;

    static constexpr auto type_list_by_channel = mpdef::make_map(
      mpdef::make_pair(hana::type_c<upstream>, hana::type_c<UpstreamTypes>),
      mpdef::make_pair(hana::type_c<downstream>, hana::type_c<DownstreamTypes>)
    );

    template <typename M, typename Payload, typename IsFromRoot>
    decltype(auto) to_downstream_helper(M const& m, Payload const& payload, IsFromRoot) const
    {
      using Message = typename decltype(get_message_type(
        downstream{},
        message::get_action(m),
        message::get_path(m)
      ))::type;

      // truncate trailing nothings
      return hana::unpack(
        hana::drop_back(
          hana::make_tuple(
            std::cref(message::get_path(m)),
            std::cref(message::get_maybe_uid(m)),
            detail::make_is_from_root_fn<Message>{}(IsFromRoot{}),
            std::cref(payload)
          ),
          decltype(hana::size_c<6> - hana::length(std::declval<Message>())){}
        ),
        [&](auto ...x)
        {
          return Message(
            downstream{},
            message::get_action(m),
            x...
          );
        }
      );
        
    }

    public:

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

    template <typename SystemMessage>
    constexpr auto get_upstream_variant_type() const
    {
      return hana::unpack(UpstreamTypes{},
        hana::partial(hana::template_<nbdl::variant>, hana::type_c<SystemMessage>));
    }

    template <typename SystemMessage>
    constexpr auto get_downstream_variant_type() const
    {
      return hana::unpack(DownstreamTypes{},
        hana::partial(hana::template_<nbdl::variant>, hana::type_c<SystemMessage>));
    }

    template <typename SystemMessage, typename Message>
    auto make_upstream_variant(Message&& m) const
    {
      using Variant = typename decltype(get_upstream_variant_type<SystemMessage>())::type;
      return Variant(std::forward<Message>(m));
    }

    template <typename SystemMessage, typename Message>
    auto make_downstream_variant(Message&& m) const
    {
      using Variant = typename decltype(get_downstream_variant_type<SystemMessage>())::type;
      return Variant(std::forward<Message>(m));
    }

    template <typename Channel, typename Action, typename Path, typename ...T>
    auto make_message(Channel const& channel, Action const& action, Path&& path, T&& ...t) const
    {
      using Message = typename decltype(get_message_type(channel, action, path))::type;
      using UidOrNothing = decltype(hana::if_(
        hana::is_just(message::get_maybe_uid(std::declval<Message>())),
        nbdl::uid{},
        hana::nothing
      ));
      return Message(
        channel,
        action,
        std::forward<Path>(path),
        UidOrNothing{},
        std::forward<T>(t)... // <== is_from_root, payload and private payload
      );
    }

    // Create
    template <typename Path, typename ...T>
    decltype(auto) make_upstream_create_message(Path&& p, T&& ...t) const
    {
      return make_message(upstream{}, create{}, std::forward<Path>(p),
        std::forward<T>(t)...);
    }

    template <typename Path, typename ...T>
    decltype(auto) make_downstream_create_message(Path&& p, T&& ...t) const
    {
      return make_message(downstream{}, create{}, std::forward<Path>(p),
        false, std::forward<T>(t)...);
    }

    template <typename Path, typename ...T>
    decltype(auto) make_downstream_create_message(nbdl::from_root r, Path&& p, T&& ...t) const
    {
      return make_message(downstream{}, create{}, std::forward<Path>(p),
        r.is_from_root, std::forward<T>(t)...);
    }

    // Read
    template <typename Path, typename ...T>
    decltype(auto) make_upstream_read_message(Path&& p, T&& ...t) const
    {
      return make_message(upstream{}, read{}, std::forward<Path>(p), std::forward<T>(t)...);
    }

    template <typename Path, typename ...T>
    decltype(auto) make_downstream_read_message(Path&& p, T&& ...t) const
    {
      return make_message(downstream{}, read{}, std::forward<Path>(p),
        false, std::forward<T>(t)...);
    }

    template <typename Path, typename ...T>
    decltype(auto) make_downstream_read_message(nbdl::from_root r, Path&& p, T&& ...t)
    {
      return make_message(downstream{}, read{}, std::forward<Path>(p),
        r.is_from_root, std::forward<T>(t)...);
    }

    // Update
    template <typename Path, typename ...T>
    decltype(auto) make_upstream_update_message(Path&& p, T&& ...t) const
    {
      return make_message(upstream{}, update{}, std::forward<Path>(p), std::forward<T>(t)...);
    }

    template <typename Path, typename ...T>
    decltype(auto) make_downstream_update_message(Path&& p, T&& ...t) const
    {
      return make_message(downstream{}, update{}, std::forward<Path>(p),
        false, std::forward<T>(t)...);
    }

    template <typename Path, typename ...T>
    decltype(auto) make_downstream_update_message(nbdl::from_root r, Path&& p, T&& ...t) const
    {
      return make_message(downstream{}, update{}, std::forward<Path>(p),
        r.is_from_root, std::forward<T>(t)...);
    }

    // UpdateRaw
    template <typename Path, typename ...T>
    decltype(auto) make_upstream_update_raw_message(Path&& p, T&& ...t) const
    {
      return make_message(upstream{}, update_raw{}, std::forward<Path>(p), std::forward<T>(t)...);
    }

    template <typename Path, typename ...T>
    decltype(auto) make_downstream_update_raw_message(Path&& p, T&& ...t) const
    {
      return make_message(downstream{}, update_raw{}, std::forward<Path>(p),
        false, std::forward<T>(t)...);
    }

    template <typename Path, typename ...T>
    decltype(auto) make_downstream_update_raw_message(nbdl::from_root r, Path&& p, T&& ...t) const
    {
      return make_message(downstream{}, update_raw{}, std::forward<Path>(p),
        r.is_from_root, std::forward<T>(t)...);
    }

    // Delete
    template <typename Path, typename ...T>
    decltype(auto) make_upstream_delete_message(Path&& p, T&& ...t) const
    {
      return make_message(upstream{}, delete_{}, std::forward<Path>(p), std::forward<T>(t)...);
    }

    template <typename Path, typename ...T>
    decltype(auto) make_downstream_delete_message(Path&& p, T&& ...t) const
    {
      return make_message(downstream{}, delete_{}, std::forward<Path>(p),
        false, std::forward<T>(t)...);
    }

    template <typename Path, typename ...T>
    decltype(auto) make_downstream_delete_message(nbdl::from_root r, Path&& p, T&& ...t) const
    {
      return make_message(downstream{}, delete_{}, std::forward<Path>(p),
        r.is_from_root, std::forward<T>(t)...);
    }

    // Convert to Downstream

    template <typename Message>
    decltype(auto) to_downstream(Message const& m) const
    {
      return to_downstream_helper(m, message::get_maybe_payload(m), hana::false_c);
    }

    template <typename Message, typename Payload>
    decltype(auto) to_downstream(Message && m, Payload&& p) const
    {
      return to_downstream_helper(std::forward<Message>(m), std::forward<Payload>(p), hana::false_c);
    }

    template <typename Message>
    decltype(auto) to_downstream_from_root(Message const& m) const
    {
      return to_downstream_helper(m, message::get_maybe_payload(m), hana::true_c);
    }

    template <typename Message, typename Payload>
    decltype(auto) to_downstream_from_root(Message && m, Payload&& p) const
    {
      return to_downstream_helper(std::forward<Message>(m), std::forward<Payload>(p), hana::true_c);
    }
  };
} // nbdl

#endif
