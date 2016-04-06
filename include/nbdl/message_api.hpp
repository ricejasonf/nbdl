//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MESSAGE_API_HPP
#define NBDL_MESSAGE_API_HPP

#include <nbdl/message.hpp>

#include <boost/hana/experimental/types.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana/traits.hpp>

namespace nbdl
{
  namespace hana = boost::hana;
  namespace hanax = boost::hana::experimental;

  namespace detail
  {
    template <typename Channel, typename Action, typename Path>
    struct message_lookup_predicate_fn
    {
      template <typename Message>
      constexpr auto operator()(Message const& m)
      {
        return
              hana::type_c<decltype(message::get_channel(m))> == hana::type_c<Channel>
          &&  hana::type_c<decltype(message::get_action(m))>  == hana::type_c<Action>
          &&  message::get_path_type(m) == hana::decay(hana::type_c<Path>);
          ;
      }
    }
  }

  template <typename UpstreamTypes, typename DownstreamTypes>
  struct message_api
  {
    using Upstream = messagee::channel::upstream;
    using Downstream = messagee::channel::downstream;

    template <typename Channel>
    struct type_list_by_channel;

    template <>
    struct type_list_by_channel<Upstream>
    { using type = UpstreamTypes; }

    template <>
    struct type_list_by_channel<Downstream>
    { using type = DownstreamTypes; }

    template <typename Channel, typename Action, typename Path>
    constexpr auto get_message_type(Channel, Action, Path)
    {
      constexpr auto message_type = hana::find(typename type_list_by_channel<Channel>::type,
        detail::message_lookup_predicate_fn<Channel, Action, Path>{});

      static_assert(decltype(hana::is_just(message_type))::value
        , "This message API does not support the channel/action for this path.")

      return *message_type;
    }

    template <typename ...T>
    auto make_message(Channel const& channel, Action const& action, Path&& path, T&& ...t)
    {
      using MessageType = typename decltype(get_message_type(channel, action, path))::type;
      return MessageType(
        channel,
        action,
        std::forward<Path>(path),
        nbdl::uid{},
        false,
        std::forward<T>(t)... // <== payload and private payload
      );
    }

    // Create
    template <typename ...T>
    decltype(auto) make_upstream_create_message(T&& ...t)
    { return make_message(Upstream, message::action::create, std::forward<T>(t)...); }

    template <typename ...T>
    decltype(auto) make_downstream_create_message(T&& ...t)
    { return make_message(Downstream, message::action::create, std::forward<T>(t)...); }

    // Read
    template <typename ...T>
    decltype(auto) make_upstream_read_message(T&& ...t)
    { return make_message(Upstream, message::action::read, std::forward<T>(t)...); }

    template <typename ...T>
    decltype(auto) make_downstream_read_message(T&& ...t)
    { return make_message(Downstream, message::action::read, std::forward<T>(t)...); }

    // Update
    template <typename ...T>
    decltype(auto) make_upstream_update_message(T&& ...t)
    { return make_message(Upstream, message::action::update, std::forward<T>(t)...); }

    template <typename ...T>
    decltype(auto) make_downstream_update_message(T&& ...t)
    { return make_message(Downstream, message::action::update, std::forward<T>(t)...); }

    // UpdateRaw
    template <typename ...T>
    decltype(auto) make_upstream_update_raw_message(T&& ...t)
    { return make_message(Upstream, message::action::update_raw, std::forward<T>(t)...); }

    template <typename ...T>
    decltype(auto) make_downstream_update_raw_message(T&& ...t)
    { return make_message(Downstream, message::action::update_raw, std::forward<T>(t)...); }

    // Delete
    template <typename ...T>
    decltype(auto) make_upstream_delete_message(T&& ...t)
    { return make_message(Upstream, message::action::delete_, std::forward<T>(t)...); }

    template <typename ...T>
    decltype(auto) make_downstream_delete_message(T&& ...t)
    { return make_message(Downstream, message::action::delete_, std::forward<T>(t)...); }
  };
} // nbdl

#endif
