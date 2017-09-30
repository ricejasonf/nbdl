//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_MESSAGE_HPP
#define NBDL_FWD_MESSAGE_HPP

#include <type_traits>
#include <utility>

namespace nbdl::message
{
  template <typename...>
  struct upstream_create;

  template <typename...>
  struct upstream_read;

  template <typename...>
  struct upstream_update;

  template <typename...>
  struct upstream_delete;

  template <typename...>
  struct downstream_create;

  template <typename...>
  struct downstream_read;

  template <typename...>
  struct downstream_update;

  template <typename...>
  struct downstream_delete;

  /*
   * The action and channel tags serve to
   * indicate how the message should be
   * dispatched.
   */

  namespace action
  {
    struct create { };
    struct read { };
    struct update { };
    struct delete_ { };
  }

  namespace channel
  {
    struct upstream { };
    struct downstream { };
  }

  // channel/action traits
  template <typename Message> constexpr bool is_upstream_impl         = false;
  template <typename Message> constexpr bool is_downstream_impl       = false;
  template <typename Message> constexpr bool is_create_impl           = false;
  template <typename Message> constexpr bool is_read_impl             = false;
  template <typename Message> constexpr bool is_update_impl           = false;
  template <typename Message> constexpr bool is_delete_impl           = false;

  template <typename Message> constexpr bool is_upstream    = is_upstream_impl<std::decay_t<Message>>;
  template <typename Message> constexpr bool is_downstream  = is_downstream_impl<std::decay_t<Message>>;
  template <typename Message> constexpr bool is_create      = is_create_impl<std::decay_t<Message>>;
  template <typename Message> constexpr bool is_read        = is_read_impl<std::decay_t<Message>>;
  template <typename Message> constexpr bool is_update      = is_update_impl<std::decay_t<Message>>;
  template <typename Message> constexpr bool is_delete      = is_delete_impl<std::decay_t<Message>>;

  struct no_impl { };

  // impls - See corresponding *_fn for info
  template <typename Message, typename = void> constexpr auto get_channel_impl            = no_impl{};
  template <typename Message, typename = void> constexpr auto get_action_impl             = no_impl{};
  template <typename Message, typename = void> constexpr auto get_path_impl               = no_impl{};
  template <typename Message, typename = void> constexpr auto get_uid_impl                = no_impl{};
  template <typename Message, typename = void> constexpr auto is_confirmed_impl           = no_impl{};
  template <typename Message, typename = void> constexpr auto get_payload_impl            = no_impl{};
  template <typename Message, typename = void> constexpr auto get_private_payload_impl    = no_impl{};
  template <typename Message, typename = void> constexpr auto to_downstream_impl          = no_impl{};
  template <typename Message, typename = void> constexpr auto to_downstream_with_key_impl = no_impl{};
  template <typename Message, typename = void> constexpr auto to_downstream_read_impl     = no_impl{};
  template <typename Message, typename = void>
  constexpr auto get_normalized_path_impl = get_path_impl<Message>;

  struct get_channel_fn
  {
    template <typename Message>
    constexpr auto operator()(Message&&) const;
  };

  struct get_action_fn
  {
    template <typename Message>
    constexpr auto operator()(Message&&) const;
  };

  struct get_path_fn
  {
    template <typename Message>
    constexpr auto operator()(Message&&) const;
  };

  struct get_uid_fn
  {
    template <typename Message>
    constexpr auto operator()(Message&& m) const
      -> decltype(get_uid_impl<std::decay_t<Message>>(std::forward<Message>(m)));
  };

  struct is_confirmed_fn
  {
    template <typename Message>
    constexpr auto operator()(Message&& m) const
      -> decltype(is_confirmed_impl<std::decay_t<Message>>(std::forward<Message>(m)));
  };

  struct get_payload_fn
  {
    template <typename Message>
    constexpr auto operator()(Message&&) const;
  };

  struct get_private_payload_fn
  {
    template <typename Message>
    constexpr auto operator()(Message&& m) const
      -> decltype(get_private_payload_impl<std::decay_t<Message>>(std::forward<Message>(m)));
  };

  struct get_normalized_path_fn
  {
    template <typename Message>
    constexpr auto operator()(Message&& m) const
      -> decltype(get_normalized_path_impl<std::decay_t<Message>>(std::forward<Message>(m)));
  };

  struct to_downstream_fn
  {
    template <typename Message, typename IsConfirmed>
    constexpr auto operator()(Message&&, IsConfirmed is_confirmed) const;
  };

  struct to_downstream_with_key_fn
  {
    template <typename Message, typename Key>
    constexpr auto operator()(Message&&, Key&&) const;
  };

  struct to_downstream_with_key_confirmed_fn
  {
    template <typename Message, typename Key>
    constexpr auto operator()(Message&&, Key&&) const;
  };

  struct to_downstream_read_fn
  {
    template <typename Message, typename Payload>
    constexpr auto operator()(Message&&, Payload&&) const;
  };

  constexpr get_channel_fn                       get_channel{};
  constexpr get_action_fn                        get_action{};
  constexpr get_path_fn                          get_path{};
  constexpr get_uid_fn                           get_uid{};
  constexpr is_confirmed_fn                      is_confirmed{};
  constexpr get_payload_fn                       get_payload{};
  constexpr get_private_payload_fn               get_private_payload{};
  constexpr get_normalized_path_fn               get_normalized_path{};
  constexpr to_downstream_fn                     to_downstream{};
  constexpr to_downstream_with_key_fn            to_downstream_with_key{};
  constexpr to_downstream_with_key_confirmed_fn  to_downstream_with_key_confirmed{};
  constexpr to_downstream_read_fn                to_downstream_read{};

  constexpr auto to_downstream_confirmed = [](auto&& m)
  {
    return to_downstream(std::forward<decltype(m)>(m), true);
  };

  template <typename Message>
  constexpr bool has_uid = !std::is_same<no_impl, std::decay_t<decltype(get_uid_impl<Message>)>>::value;

  template <typename Message>
  constexpr bool has_is_confirmed = !std::is_same<no_impl, std::decay_t<decltype(is_confirmed_impl<Message>)>>::value;

  template <typename Message>
  constexpr bool has_payload = !std::is_same<no_impl, std::decay_t<decltype(get_payload_impl<Message>)>>::value;

  template <typename Message>
  constexpr bool has_private_payload = !std::is_same<no_impl, std::decay_t<decltype(get_uid_impl<Message>)>>::value;

  template <typename Message>
  constexpr bool is_path_normalizable = !std::is_same<no_impl,
    std::decay_t<decltype(get_normalized_path_impl<Message>)>>::value;

  // no_uid - readable param for no_impl

  constexpr no_impl no_uid{};

  // no_is_confirmed - readable param for no_impl

  constexpr no_impl no_is_confirmed{};

  // make functions

  struct make_upstream_create_fn
  {
    template <typename Path, typename Uid, typename Payload, typename PrivatePayload>
    auto operator()(Path&&, Uid&&, Payload&&, PrivatePayload&&) const;

    template <typename Path, typename Uid, typename Payload>
    auto operator()(Path&&, Uid&&, Payload&&) const;
  };

  struct make_upstream_read_fn
  {
    template <typename Path, typename Uid>
    auto operator()(Path&&, Uid&&) const;

    template <typename Path>
    auto operator()(Path&& path) const;
  };

  struct make_upstream_update_fn
  {
    template <typename Path, typename Uid, typename Payload>
    auto operator()(Path&&, Uid&&, Payload&&) const;
  };

  struct make_upstream_delete_fn
  {
    template <typename Path, typename Uid>
    auto operator()(Path&&, Uid&&) const;
  };

  struct make_downstream_create_fn
  {
    template <typename Path, typename Uid, typename Payload, typename IsConfirmed>
    auto operator()(Path&&, Uid&&, Payload&&, IsConfirmed) const;
  };

  struct make_downstream_read_fn
  {
    template <typename Path, typename Uid, typename Payload>
    auto operator()(Path&&, Uid&&, Payload&&) const;
  };

  struct make_downstream_update_fn
  {
    template <typename Path, typename Uid, typename Payload, typename IsConfirmed>
    auto operator()(Path&&, Uid&&, Payload&&, IsConfirmed) const;
  };

  struct make_downstream_delete_fn
  {
    template <typename Path, typename Uid, typename IsConfirmed>
    auto operator()(Path&&, Uid&&, IsConfirmed) const;
  };

  constexpr make_upstream_create_fn make_upstream_create{};
  constexpr make_upstream_read_fn   make_upstream_read{};
  constexpr make_upstream_update_fn make_upstream_update{};
  constexpr make_upstream_delete_fn make_upstream_delete{};
  constexpr make_downstream_create_fn make_downstream_create{};
  constexpr make_downstream_read_fn   make_downstream_read{};
  constexpr make_downstream_update_fn make_downstream_update{};
  constexpr make_downstream_delete_fn make_downstream_delete{};

  // make_create_path<Key>

  template <typename Key>
  struct make_create_path_fn
  {
    template <typename ParentPath>
    constexpr auto operator()(ParentPath&&) const;
  };

  template <typename Key>
  constexpr make_create_path_fn<Key> make_create_path{};

  // get_path_type

  struct get_path_type_fn
  {
    template <typename Message>
    constexpr auto operator()(Message const& m) const;
  };

  constexpr get_path_type_fn get_path_type{};
}

#endif
