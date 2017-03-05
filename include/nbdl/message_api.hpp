//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MESSAGE_API_HPP
#define NBDL_MESSAGE_API_HPP

#include <mpdef/map.hpp>
#include <mpdef/pair.hpp>
#include <nbdl/detail/normalize_path_type.hpp>
#include <nbdl/make_store.hpp> // nbdl::not_found
#include <nbdl/message.hpp>
#include <nbdl/uid.hpp>
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
    // Converts the path to not "create path" and assigns it the provided key
    template <typename Path, typename Key>
    auto canonicalize_path(Path&& p, Key&& k)
    {
      constexpr auto path_type = decltype(normalize_path_type(hana::typeid_(p))){};
      using CanonicalPath = typename decltype(path_type)::type;

      return hana::unpack(
        hana::drop_back(std::forward<Path>(p), hana::size_c<1>),
        [&](auto&& ...xs)
        {
          return CanonicalPath(
            std::forward<decltype(xs)>(xs)...,
            std::forward<Key>(k)
          );
        }
      );
    }

    template <typename P, typename MaybeUid>
    decltype(auto) canonicalize_path_with_maybe_uid(P&& p, MaybeUid&& k)
    {
      constexpr auto path_type = decltype(normalize_path_type(hana::typeid_(p))){};

      // If the path is a "create path" use the uid as the key if present.
      if constexpr(
            decltype(hana::is_just(k))::value
        &&  decltype(path_type != hana::typeid_(p))::value
      )
      {
        // The value_or is used to make it dependent.
        return canonicalize_path(
          std::forward<P>(p),
          std::forward<MaybeUid>(k).value_or(p)
        );
      }
      else
      {
        return std::forward<P>(p);
      }
    }

    template <typename Channel, typename Action, typename Path, typename ...>
    struct message_lookup_predicate_fn;

    template <typename Channel, typename Action, typename Path>
    struct message_lookup_predicate_fn<Channel, Action, Path>
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

    template <typename Channel, typename Action, typename Path, typename Payload>
    struct message_lookup_predicate_fn<Channel, Action, Path, Payload>
    {
      template <typename MessageType>
      constexpr auto operator()(MessageType const&)
      {
        using hana::traits::decay;
        using M = typename MessageType::type;
        return
              message_lookup_predicate_fn<Channel, Action, Path>{}(MessageType{})
          &&  decay(hana::type_c<decltype(message::get_maybe_payload(std::declval<M>()))>)
                == decay(hana::type_c<decltype(hana::just(std::declval<Payload>()))>)
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

    template <typename Message, typename M, typename Path, typename Payload, typename IsFromRoot>
    decltype(auto) to_downstream_helper_helper(
      M const& m,
      Path const& path,
      Payload const& payload,
      IsFromRoot
    )
    {
      constexpr auto is_from_root = detail::make_is_from_root_fn<Message>{}(IsFromRoot{});
      // truncate to length of Message
      return hana::unpack(
        hana::drop_back(
          hana::make_tuple(
            std::cref(path),
            std::cref(message::get_maybe_uid(m)),
            std::cref(is_from_root),
            std::cref(payload)
          ),
          decltype(hana::size_c<6> - hana::length(std::declval<Message>())){}
        ),
        [&](auto ...ref)
        {
          return Message(
            message::channel::downstream{},
            message::get_action(m),
            ref.get()...
          );
        }
      );
    }

  } // detail

  // wrapper for safe overloading
  struct from_root
  {
    bool is_from_root;
  };

  template <typename Context>
  class message_api
  {
    using UpstreamTypes   = typename Context::message_api_meta::upstream_types;
    using DownstreamTypes = typename Context::message_api_meta::downstream_types;

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

    template <typename M, typename Path, typename Payload, typename IsFromRoot>
    decltype(auto) to_downstream_helper(
      M&& m,
      Path&& path,
      Payload&& payload,
      IsFromRoot
    ) const
    {
      using Message = typename decltype(get_message_type(
        downstream{},
        message::get_action(m),
        detail::canonicalize_path_with_maybe_uid(
          std::forward<Path>(path),
          message::get_maybe_uid(m)
        ),
        payload
      ))::type;

      // `canonicalize_path` idempotently sets the path key
      // to the uid if something isn't already specified.
      return detail::to_downstream_helper_helper<Message>(
        std::forward<M>(m),
        detail::canonicalize_path_with_maybe_uid(
          std::forward<Path>(path),
          message::get_maybe_uid(m)
        ),
        std::forward<Payload>(payload),
        IsFromRoot{}
      );
    }

    template <typename Channel, typename Pred>
    constexpr auto find_message_type_helper(Channel, Pred pred) const
    {
      using Messages = typename decltype(
        hana::at_key(type_list_by_channel, hana::type_c<Channel>)
      )::type;
      return hana::find_if(Messages{}, pred);
    }

    template <typename Channel, typename Pred>
    constexpr auto get_message_type_helper(Channel, Pred pred) const
    {
      constexpr auto message_type = decltype(find_message_type_helper(Channel{}, pred)){};

      static_assert(decltype(hana::is_just(message_type))::value
        , "This message API does not support the channel/action for this path.");

      return *message_type;
    }

    public:

    template <typename Path>
    constexpr auto has_upstream_read(Path) const
    {
      return hana::is_just(find_message_type_helper(upstream{},
        detail::message_lookup_predicate_fn<upstream, read, Path>{}));
    }

    // The fourth parameter would be the payload.
    template <typename Path>
    constexpr auto get_message_type(downstream, read, Path, nbdl::not_found) const
    {
      return get_message_type_helper(downstream{}, 
        detail::message_lookup_predicate_fn<downstream, read, Path, nbdl::not_found>{});
    }

    template <typename Channel, typename Action, typename Path>
    constexpr auto get_message_type(Channel, Action, Path, ...) const
    {
      return get_message_type_helper(Channel{}, 
        detail::message_lookup_predicate_fn<Channel, Action, Path>{});
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

    // Create - Upstream(Path, Payload, PrivatePayload)
    template <typename Path, typename ...T>
    auto make_upstream_create_message(Path&& p, T&& ...t) const
    {
      return make_message(upstream{}, create{}, std::forward<Path>(p),
        std::forward<T>(t)...);
    }

    template <typename Path, typename ...T>
    auto make_downstream_create_message(Path&& p, T&& ...t) const
    {
      return make_message(downstream{}, create{}, std::forward<Path>(p),
        false, std::forward<T>(t)...);
    }

    template <typename Path, typename ...T>
    auto make_downstream_create_message(nbdl::from_root r, Path&& p, T&& ...t) const
    {
      return make_message(downstream{}, create{}, std::forward<Path>(p),
        r.is_from_root, std::forward<T>(t)...);
    }

    // Read
    template <typename Path, typename ...T>
    auto make_upstream_read_message(Path&& p, T&& ...t) const
    {
      return make_message(upstream{}, read{}, std::forward<Path>(p), std::forward<T>(t)...);
    }

    template <typename Path, typename ...T>
    auto make_downstream_read_message(Path&& p, T&& ...t) const
    {
      return make_message(downstream{}, read{}, std::forward<Path>(p),
        false, std::forward<T>(t)...);
    }

    template <typename Path, typename ...T>
    auto make_downstream_read_message(nbdl::from_root r, Path&& p, T&& ...t)
    {
      return make_message(downstream{}, read{}, std::forward<Path>(p),
        r.is_from_root, std::forward<T>(t)...);
    }

    // Update
    template <typename Path, typename ...T>
    auto make_upstream_update_message(Path&& p, T&& ...t) const
    {
      return make_message(upstream{}, update{}, std::forward<Path>(p), std::forward<T>(t)...);
    }

    template <typename Path, typename ...T>
    auto make_downstream_update_message(Path&& p, T&& ...t) const
    {
      return make_message(downstream{}, update{}, std::forward<Path>(p),
        false, std::forward<T>(t)...);
    }

    template <typename Path, typename ...T>
    auto make_downstream_update_message(nbdl::from_root r, Path&& p, T&& ...t) const
    {
      return make_message(downstream{}, update{}, std::forward<Path>(p),
        r.is_from_root, std::forward<T>(t)...);
    }

    // UpdateRaw
    template <typename Path, typename ...T>
    auto make_upstream_update_raw_message(Path&& p, T&& ...t) const
    {
      return make_message(upstream{}, update_raw{}, std::forward<Path>(p), std::forward<T>(t)...);
    }

    template <typename Path, typename ...T>
    auto make_downstream_update_raw_message(Path&& p, T&& ...t) const
    {
      return make_message(downstream{}, update_raw{}, std::forward<Path>(p),
        false, std::forward<T>(t)...);
    }

    template <typename Path, typename ...T>
    auto make_downstream_update_raw_message(nbdl::from_root r, Path&& p, T&& ...t) const
    {
      return make_message(downstream{}, update_raw{}, std::forward<Path>(p),
        r.is_from_root, std::forward<T>(t)...);
    }

    // Delete
    template <typename Path, typename ...T>
    auto make_upstream_delete_message(Path&& p, T&& ...t) const
    {
      return make_message(upstream{}, delete_{}, std::forward<Path>(p), std::forward<T>(t)...);
    }

    template <typename Path, typename ...T>
    auto make_downstream_delete_message(Path&& p, T&& ...t) const
    {
      return make_message(downstream{}, delete_{}, std::forward<Path>(p),
        false, std::forward<T>(t)...);
    }

    template <typename Path, typename ...T>
    auto make_downstream_delete_message(nbdl::from_root r, Path&& p, T&& ...t) const
    {
      return make_message(downstream{}, delete_{}, std::forward<Path>(p),
        r.is_from_root, std::forward<T>(t)...);
    }

    // Convert to Downstream

    template <typename Message>
    auto to_downstream(Message const& m) const
    {
      return to_downstream_helper(
        m,
        message::get_path(m),
        message::get_maybe_payload(m),
        hana::false_c
      );
    }

    template <typename Message, typename Payload>
    auto to_downstream(Message const& m, Payload&& p) const
    {
      return to_downstream_helper(
        m,
        message::get_path(m),
        std::forward<Payload>(p),
        hana::false_c
      );
    }

    // TODO:
    // If a message is coming from root, the caller must specify the value of the key.
    // (for create messages)
    template <typename Message>
    auto to_downstream_from_root(Message const& m) const
    {
      return to_downstream_helper(
        m,
        message::get_path(m),
        message::get_maybe_payload(m),
        hana::true_c
      );
    }

    template <typename Message, typename Key>
    auto to_downstream_from_root(Message const& m, Key&& k) const
    {
      return to_downstream_helper(
        m,
        detail::canonicalize_path(message::get_path(m), std::forward<Key>(k)),
        message::get_maybe_payload(m),
        hana::true_c
      );
    }

    template <typename Message, typename Payload>
    auto to_downstream_from_root_with_payload(Message const& m, Payload&& p) const
    {
      return to_downstream_helper(
        m,
        message::get_path(m),
        std::forward<Payload>(p),
        hana::true_c
      );
    }

    template <typename Message, typename Key, typename Payload>
    auto to_downstream_from_root_with_payload(Message const& m, Key&& k, Payload&& p) const
    {
      return to_downstream_helper(
        m,
        detail::canonicalize_path(message::get_path(m), std::forward<Key>(k)),
        std::forward<Payload>(p),
        hana::true_c
      );
    }
  };
} // nbdl

#endif
