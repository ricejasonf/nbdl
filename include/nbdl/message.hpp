//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MESSAGE_HPP
#define NBDL_MESSAGE_HPP

#include <nbdl/bind_sequence.hpp>
#include <nbdl/detail/normalize_path_type.hpp>
#include <nbdl/fwd/message.hpp>

#include <boost/hana/append.hpp>
#include <boost/hana/core/tag_of.hpp>
#include <boost/hana/basic_tuple.hpp>
#include <boost/hana/optional.hpp>
#include <type_traits>
#include <utility>

#define NBDL_DETAIL_MESSAGE_ACCESSOR_IMPL(IMPL_TYPE, NAME, I) \
  template <typename ...T> \
  constexpr auto NAME<IMPL_TYPE<T...>> = [](auto&& x) { \
    static_assert( \
      decltype(hana::typeid_(hana::at_c<I>(x.storage)) != hana::type_c<no_impl>)::value \
    , #IMPL_TYPE " must have an implementation for " #NAME \
    ); \
    return hana::at_c<I>(std::forward<decltype(x)>(x).storage); \
  };

#define NBDL_DETAIL_MESSAGE_OPTIONAL_ACCESSOR_IMPL(IMPL_TYPE, NAME, I) \
  template <typename ...T> \
  constexpr auto NAME< \
    IMPL_TYPE<T...> \
  , std::enable_if_t<!std::is_same< \
      no_impl \
    , std::decay_t<decltype(hana::at_c<I>(std::declval<IMPL_TYPE<T...>>().storage))> \
    >::value> \
  > = [](auto&& x) \
  { return hana::at_c<I>(std::forward<decltype(x)>(x).storage); };

namespace nbdl::message
{
  namespace hana = boost::hana;

  namespace detail
  {
    constexpr auto get_normalized_create_path = [](auto&& m)
      -> decltype(hana::append(
           get_path(m).parent_path
         , typename std::decay_t<decltype(get_path(m))>::key{
            get_uid(m)
           }
         ))
    {
      return hana::append(
        get_path(std::forward<decltype(m)>(m)).parent_path
      , get_uid(std::forward<decltype(m)>(m))
      );
    };

    template <typename Message>
    constexpr bool is_create_path_normalizable = decltype(
      hana::is_valid(get_normalized_create_path, std::declval<Message>())
    )::value;
  }

  // upstream_create

  template <typename Path, typename Uid, typename Payload, typename PrivatePayload>
  struct upstream_create<Path, Uid, Payload, PrivatePayload>
  {
    using storage_t = hana::basic_tuple<
      Path
    , Uid
    , Payload
    , PrivatePayload
    >;

    storage_t storage;
  };

  NBDL_DETAIL_MESSAGE_ACCESSOR_IMPL(upstream_create, get_path_impl, 0);
  NBDL_DETAIL_MESSAGE_OPTIONAL_ACCESSOR_IMPL(upstream_create, get_uid_impl, 1);
  NBDL_DETAIL_MESSAGE_ACCESSOR_IMPL(upstream_create, get_payload_impl, 2);
  NBDL_DETAIL_MESSAGE_OPTIONAL_ACCESSOR_IMPL(upstream_create, get_private_payload_impl, 1);

  template <typename ...T>
  constexpr auto get_channel_impl<upstream_create<T...>> = channel::upstream{};

  template <typename ...T>
  constexpr auto get_action_impl<upstream_create<T...>> = action::create{};

  template <typename M>
  constexpr bool requires_key_for_downstream_impl<M,
    std::enable_if_t<hana::is_a<create_path_tag, decltype(get_path(std::declval<M>()))>
                 and not detail::is_create_path_normalizable<M>
                  >> = true;

  template <typename ...T>
  constexpr auto get_normalized_path_impl<
    upstream_create<T...>
  , std::enable_if_t<requires_key_for_downstream<upstream_create<T...>>
  >
  > = no_impl{};

  template <typename ...T>
  constexpr auto get_normalized_path_impl<
    upstream_create<T...>
  , std::enable_if_t<detail::is_create_path_normalizable<upstream_create<T...>>>
  > = [](auto&& m)
  {
    return detail::get_normalized_create_path(std::forward<decltype(m)>(m));
  };

  template <typename ...T>
  constexpr auto to_downstream_impl<
    upstream_create<T...>
  , std::enable_if_t<is_path_normalizable<upstream_create<T...>>
             and not requires_key_for_downstream<upstream_create<T...>>>
  > = [](auto&& m, auto is_confirmed)
  {
    using M = decltype(m);

    return make_downstream_create(
      get_path(std::forward<M>(m))
    , hana::sfinae(get_uid)(std::forward<M>(m)).value_or(no_impl{})
    , get_payload(std::forward<M>(m))
    , is_confirmed
    );
  };

  template <typename ...T>
  constexpr auto to_downstream_with_key_impl<upstream_create<T...>> = [](auto&& m
                                                                       , auto&& k
                                                                       , auto is_confirmed)
  {
    using M = decltype(m);
    using Key = typename std::decay_t<decltype(get_path(m))>::key;

    return make_downstream_create(
      hana::append(get_path(std::forward<M>(m)).parent_path, Key{std::forward<decltype(k)>(k)})
    , hana::sfinae(get_uid)(std::forward<M>(m)).value_or(no_impl{})
    , get_payload(std::forward<M>(m))
    , is_confirmed
    );
  };

  // upstream_read

  template <typename Path, typename Uid>
  struct upstream_read<Path, Uid>
  {
    using storage_t = hana::basic_tuple<
      Path
    , Uid
    >;

    storage_t storage;
  };

  NBDL_DETAIL_MESSAGE_ACCESSOR_IMPL(upstream_read, get_path_impl, 0);
  NBDL_DETAIL_MESSAGE_OPTIONAL_ACCESSOR_IMPL(upstream_read, get_uid_impl, 1);

  template <typename ...T>
  constexpr auto get_channel_impl<upstream_read<T...>> = channel::upstream{};

  template <typename ...T>
  constexpr auto get_action_impl<upstream_read<T...>> = action::read{};

  template <typename ...T>
  constexpr auto to_downstream_read_impl<upstream_read<T...>> = [](auto&& m, auto&& payload)
  {
    using M = decltype(m);

    return make_downstream_read(
      get_path(std::forward<M>(m))
    , hana::sfinae(get_uid)(std::forward<M>(m)).value_or(no_impl{})
    , std::forward<decltype(payload)>(payload)
    );
  };

  // upstream_update

  template <typename Path, typename Uid, typename Payload>
  struct upstream_update<Path, Uid, Payload>
  {
    using storage_t = hana::basic_tuple<
      Path
    , Uid
    , Payload
    >;

    storage_t storage;
  };

  NBDL_DETAIL_MESSAGE_ACCESSOR_IMPL(upstream_update, get_path_impl, 0);
  NBDL_DETAIL_MESSAGE_OPTIONAL_ACCESSOR_IMPL(upstream_update, get_uid_impl, 1);
  NBDL_DETAIL_MESSAGE_ACCESSOR_IMPL(upstream_update, get_payload_impl, 2);

  template <typename ...T>
  constexpr auto get_channel_impl<upstream_update<T...>> = channel::upstream{};

  template <typename ...T>
  constexpr auto get_action_impl<upstream_update<T...>> = action::update{};

  template <typename ...T>
  constexpr auto to_downstream_impl<upstream_update<T...>> = [](auto&& m, auto is_confirmed)
  {
    using M = decltype(m);

    return make_downstream_update(
      get_path(std::forward<M>(m))
    , hana::sfinae(get_uid)(std::forward<M>(m)).value_or(no_impl{})
    , get_payload(std::forward<M>(m))
    , is_confirmed
    );
  };

  // upstream_delete

  template <typename Path, typename Uid>
  struct upstream_delete<Path, Uid>
  {
    using storage_t = hana::basic_tuple<Path, Uid>;

    storage_t storage;
  };

  NBDL_DETAIL_MESSAGE_ACCESSOR_IMPL(upstream_delete, get_path_impl, 0);
  NBDL_DETAIL_MESSAGE_OPTIONAL_ACCESSOR_IMPL(upstream_delete, get_uid_impl, 1);

  template <typename ...T>
  constexpr auto get_channel_impl<upstream_delete<T...>> = channel::upstream{};

  template <typename ...T>
  constexpr auto get_action_impl<upstream_delete<T...>> = action::delete_{};

  template <typename ...T>
  constexpr auto to_downstream_impl<upstream_delete<T...>> = [](auto&& m, auto is_confirmed)
  {
    using M = decltype(m);

    return make_downstream_delete(
      get_path(std::forward<M>(m))
    , hana::sfinae(get_uid)(std::forward<M>(m)).value_or(no_impl{})
    , is_confirmed
    );
  };

  // downstream_create

  template <typename Path, typename Uid, typename Payload, typename IsConfirmed>
  struct downstream_create<Path, Uid, Payload, IsConfirmed>
  {
    using storage_t = hana::basic_tuple<Path, Uid, Payload, IsConfirmed>;

    storage_t storage;
  };

  NBDL_DETAIL_MESSAGE_ACCESSOR_IMPL(downstream_create, get_path_impl, 0);
  NBDL_DETAIL_MESSAGE_OPTIONAL_ACCESSOR_IMPL(downstream_create, get_uid_impl, 1);
  NBDL_DETAIL_MESSAGE_ACCESSOR_IMPL(downstream_create, get_payload_impl, 2);
  NBDL_DETAIL_MESSAGE_OPTIONAL_ACCESSOR_IMPL(downstream_create, is_confirmed_impl, 3);

  template <typename ...T>
  constexpr auto get_channel_impl<downstream_create<T...>> = channel::downstream{};

  template <typename ...T>
  constexpr auto get_action_impl<downstream_create<T...>> = action::create{};

  template <typename Path, typename ...T>
  constexpr auto get_normalized_path_impl<
    downstream_create<Path, T...>
  , std::enable_if_t<
       !detail::is_create_path_normalizable<downstream_create<Path, T...>>
    && !hana::Sequence<Path>::value
    >
  > = no_impl{};

  template <typename ...T>
  constexpr auto get_normalized_path_impl<
    downstream_create<T...>
  , std::enable_if_t<detail::is_create_path_normalizable<downstream_create<T...>>>
  > = [](auto&& m)
  {
    return detail::get_normalized_create_path(std::forward<decltype(m)>(m));
  };

  // downstream_read

  template <typename Path, typename Uid, typename Payload>
  struct downstream_read<Path, Uid, Payload>
  {
    using storage_t = hana::basic_tuple<Path, Uid, Payload>;

    storage_t storage;
  };

  NBDL_DETAIL_MESSAGE_ACCESSOR_IMPL(downstream_read, get_path_impl, 0);
  NBDL_DETAIL_MESSAGE_OPTIONAL_ACCESSOR_IMPL(downstream_read, get_uid_impl, 1);
  NBDL_DETAIL_MESSAGE_ACCESSOR_IMPL(downstream_read, get_payload_impl, 2);
  NBDL_DETAIL_MESSAGE_OPTIONAL_ACCESSOR_IMPL(downstream_read, is_confirmed_impl, 3);

  template <typename ...T>
  constexpr auto get_channel_impl<downstream_read<T...>> = channel::downstream{};

  template <typename ...T>
  constexpr auto get_action_impl<downstream_read<T...>> = action::read{};

  // downstream_update

  template <typename Path, typename Uid, typename Payload, typename IsConfirmed>
  struct downstream_update<Path, Uid, Payload, IsConfirmed>
  {
    using storage_t = hana::basic_tuple<Path, Uid, Payload, IsConfirmed>;

    storage_t storage;
  };

  NBDL_DETAIL_MESSAGE_ACCESSOR_IMPL(downstream_update, get_path_impl, 0);
  NBDL_DETAIL_MESSAGE_OPTIONAL_ACCESSOR_IMPL(downstream_update, get_uid_impl, 1);
  NBDL_DETAIL_MESSAGE_ACCESSOR_IMPL(downstream_update, get_payload_impl, 2);
  NBDL_DETAIL_MESSAGE_OPTIONAL_ACCESSOR_IMPL(downstream_update, is_confirmed_impl, 3);

  template <typename ...T>
  constexpr auto get_channel_impl<downstream_update<T...>> = channel::downstream{};

  template <typename ...T>
  constexpr auto get_action_impl<downstream_update<T...>> = action::update{};

  // downstream_delete

  template <typename Path, typename Uid, typename IsConfirmed>
  struct downstream_delete<Path, Uid, IsConfirmed>
  {
    using storage_t = hana::basic_tuple<Path, Uid, IsConfirmed>;

    storage_t storage;
  };

  NBDL_DETAIL_MESSAGE_ACCESSOR_IMPL(downstream_delete, get_path_impl, 0);
  NBDL_DETAIL_MESSAGE_OPTIONAL_ACCESSOR_IMPL(downstream_delete, get_uid_impl, 1);
  NBDL_DETAIL_MESSAGE_OPTIONAL_ACCESSOR_IMPL(downstream_delete, is_confirmed_impl, 2);

  template <typename ...T>
  constexpr auto get_channel_impl<downstream_delete<T...>> = channel::downstream{};

  template <typename ...T>
  constexpr auto get_action_impl<downstream_delete<T...>> = action::delete_{};

  // is_upstream

  template <typename ...T> constexpr bool is_upstream_impl<upstream_create<T...>>      = true;
  template <typename ...T> constexpr bool is_upstream_impl<upstream_read<T...>>        = true;
  template <typename ...T> constexpr bool is_upstream_impl<upstream_update<T...>>      = true;
  template <typename ...T> constexpr bool is_upstream_impl<upstream_delete<T...>>      = true;

  // is_downstream

  template <typename ...T> constexpr bool is_downstream_impl<downstream_create<T...>>  = true;
  template <typename ...T> constexpr bool is_downstream_impl<downstream_read<T...>>    = true;
  template <typename ...T> constexpr bool is_downstream_impl<downstream_update<T...>>  = true;
  template <typename ...T> constexpr bool is_downstream_impl<downstream_delete<T...>>  = true;

  // is_create

  template <typename ...T> constexpr bool is_create_impl<upstream_create<T...>>   = true;
  template <typename ...T> constexpr bool is_create_impl<downstream_create<T...>> = true;

  // is_read

  template <typename ...T> constexpr bool is_read_impl<upstream_read<T...>>       = true;
  template <typename ...T> constexpr bool is_read_impl<downstream_read<T...>>     = true;

  // is_update

  template <typename ...T> constexpr bool is_update_impl<upstream_update<T...>>   = true;
  template <typename ...T> constexpr bool is_update_impl<downstream_update<T...>> = true;

  // is_delete

  template <typename ...T> constexpr bool is_delete_impl<upstream_delete<T...>>   = true;
  template <typename ...T> constexpr bool is_delete_impl<downstream_delete<T...>> = true;

  // Accessors

  template <typename Message>
  constexpr auto get_channel_fn::operator()(Message&&) const
  {
    return get_channel_impl<std::decay_t<Message>>;
  }

  template <typename Message>
  constexpr auto get_action_fn::operator()(Message&&) const
  {
    return get_action_impl<std::decay_t<Message>>;
  }

  template <typename Message>
  constexpr auto get_path_fn::operator()(Message&& m) const
  {
    return get_path_impl<std::decay_t<Message>>(std::forward<Message>(m));
  }

  template <typename Message>
  constexpr auto get_uid_fn::operator()(Message&& m) const
    -> decltype(get_uid_impl<std::decay_t<Message>>(std::forward<Message>(m)))
  {
    return get_uid_impl<std::decay_t<Message>>(std::forward<Message>(m));
  }

  template <typename Message>
  constexpr auto is_confirmed_fn::operator()(Message&& m) const
    -> decltype(is_confirmed_impl<std::decay_t<Message>>(std::forward<Message>(m)))
  {
    return is_confirmed_impl<std::decay_t<Message>>(std::forward<Message>(m));
  }

  template <typename Message>
  constexpr auto is_confirmed_fn::operator()(Message const&) const
    -> std::enable_if_t<!has_is_confirmed<Message>, hana::false_>
  {
    return hana::false_c;
  }

  template <typename Message>
  constexpr auto get_payload_fn::operator()(Message&& m) const
  {
    return get_payload_impl<std::decay_t<Message>>(std::forward<Message>(m));
  }

  template <typename Message>
  constexpr auto get_private_payload_fn::operator()(Message&& m) const
    -> decltype(get_private_payload_impl<std::decay_t<Message>>(std::forward<Message>(m)))
  {
    return get_private_payload_impl<std::decay_t<Message>>(std::forward<Message>(m));
  }

  template <typename Message>
  constexpr auto get_normalized_path_fn::operator()(Message&& m) const
    -> decltype(get_normalized_path_impl<std::decay_t<Message>>(std::forward<Message>(m)))
  {
    return get_normalized_path_impl<std::decay_t<Message>>(std::forward<Message>(m));
  }

  template <typename Message, typename IsConfirmed>
  constexpr auto to_downstream_fn::operator()(Message&& m, IsConfirmed is_confirmed) const
  {
    return to_downstream_impl<std::decay_t<Message>>(std::forward<Message>(m), is_confirmed);
  }

  template <typename Message, typename Key>
  constexpr auto to_downstream_with_key_fn::operator()(Message&& m, Key&& k) const
  {
    return to_downstream_with_key_impl<std::decay_t<Message>>(
      std::forward<Message>(m)
    , std::forward<Key>(k)
    , no_is_confirmed
    );
  }

  template <typename Message, typename Key>
  constexpr auto to_downstream_with_key_confirmed_fn::operator()(Message&& m, Key&& k) const
  {
    return to_downstream_with_key_impl<std::decay_t<Message>>(
      std::forward<Message>(m)
    , std::forward<Key>(k)
    , true // is_confirmed
    );
  }

  template <typename Message, typename Payload>
  constexpr auto to_downstream_read_fn::operator()(Message&& m, Payload&& p) const
  {
    return to_downstream_read_impl<std::decay_t<Message>>(
      std::forward<Message>(m)
    , std::forward<Payload>(p)
    );
  }

  // make functions

  template <typename Path, typename Uid, typename Payload, typename PrivatePayload>
  auto make_upstream_create_fn::operator()(Path&& path
                                         , Uid&& uid
                                         , Payload&& payload
                                         , PrivatePayload&& private_payload) const
  {
    using Message = upstream_create<
      std::decay_t<Path>
    , std::decay_t<Uid>
    , std::decay_t<Payload>
    , std::decay_t<PrivatePayload>
    >;

    return Message{hana::make_basic_tuple(
      std::forward<Path>(path)
    , std::forward<Uid>(uid)
    , std::forward<Payload>(payload)
    , std::forward<PrivatePayload>(private_payload)
    )};
  }

  template <typename Path, typename Uid, typename Payload>
  auto make_upstream_create_fn::operator()(Path&& path
                                         , Uid&& uid
                                         , Payload&& payload) const
  {
    using Message = upstream_create<
      std::decay_t<Path>
    , std::decay_t<Uid>
    , std::decay_t<Payload>
    , no_impl
    >;

    return Message{hana::make_basic_tuple(
      std::forward<Path>(path)
    , std::forward<Uid>(uid)
    , std::forward<Payload>(payload)
    , no_impl{}
    )};
  }

  template <typename Path, typename Uid>
  auto make_upstream_read_fn::operator()(Path&& path, Uid&& uid) const
  {
    using Message = upstream_read<
      std::decay_t<Path>
    , std::decay_t<Uid>
    >;

    return Message{hana::make_basic_tuple(
      std::forward<Path>(path)
    , std::forward<Uid>(uid)
    )};
  }

  template <typename Path>
  auto make_upstream_read_fn::operator()(Path&& path) const
  {
    using Message = upstream_read<
      std::decay_t<Path>
    , no_impl
    >;

    return Message{hana::make_basic_tuple(
      std::forward<Path>(path)
    , no_uid
    )};
  }

  template <typename Path, typename Uid, typename Payload>
  auto make_upstream_update_fn::operator()(Path&& path, Uid&& uid, Payload&& payload) const
  {
    using Message = upstream_update<
      std::decay_t<Path>
    , std::decay_t<Uid>
    , std::decay_t<Payload>
    >;

    return Message{hana::make_basic_tuple(
      std::forward<Path>(path)
    , std::forward<Uid>(uid)
    , std::forward<Payload>(payload)
    )};
  }

  template <typename Path, typename Uid>
  auto make_upstream_delete_fn::operator()(Path&& path, Uid&& uid) const
  {
    using Message = upstream_delete<
      std::decay_t<Path>
    , std::decay_t<Uid>
    >;

    return Message{hana::make_basic_tuple(
      std::forward<Path>(path)
    , std::forward<Uid>(uid)
    )};
  }

  template <typename Path, typename Uid, typename Payload, typename IsConfirmed>
  auto make_downstream_create_fn::operator()(Path&& path
                                           , Uid&& uid
                                           , Payload&& payload
                                           , IsConfirmed is_confirmed) const
  {
    using Message = downstream_create<
      std::decay_t<Path>
    , std::decay_t<Uid>
    , std::decay_t<Payload>
    , IsConfirmed
    >;

    return Message{hana::make_basic_tuple(
      std::forward<Path>(path)
    , std::forward<Uid>(uid)
    , std::forward<Payload>(payload)
    , is_confirmed
    )};
  }

  template <typename Path, typename Uid, typename Payload>
  auto make_downstream_read_fn::operator()(Path&& path, Uid&& uid, Payload&& payload) const
  {
    using Message = downstream_read<
      std::decay_t<Path>
    , std::decay_t<Uid>
    , std::decay_t<Payload>
    >;

    return Message{hana::make_basic_tuple(
      std::forward<Path>(path)
    , std::forward<Uid>(uid)
    , std::forward<Payload>(payload)
    )};
  }

  template <typename Path, typename Uid, typename Payload, typename IsConfirmed>
  auto make_downstream_update_fn::operator()(Path&& path
                                           , Uid&& uid
                                           , Payload&& payload
                                           , IsConfirmed is_confirmed) const
  {
    using Message = downstream_update<
      std::decay_t<Path>
    , std::decay_t<Uid>
    , std::decay_t<Payload>
    , IsConfirmed
    >;

    return Message{hana::make_basic_tuple(
      std::forward<Path>(path)
    , std::forward<Uid>(uid)
    , std::forward<Payload>(payload)
    , is_confirmed
    )};
  }

  template <typename Path, typename Uid, typename IsConfirmed>
  auto make_downstream_delete_fn::operator()(Path&& path, Uid&& uid, IsConfirmed is_confirmed) const
  {
    using Message = downstream_delete<
      std::decay_t<Path>
    , std::decay_t<Uid>
    , IsConfirmed
    >;

    return Message{hana::make_basic_tuple(
      std::forward<Path>(path)
    , std::forward<Uid>(uid)
    , is_confirmed
    )};
  }

  template <typename Key>
  template <typename ParentPath>
  constexpr auto make_create_path_fn<Key>::operator()(ParentPath&& p) const
    -> create_path<Key, std::decay_t<ParentPath>>
  {
    return {std::forward<ParentPath>(p)};
  }

  template <typename Message>
  constexpr auto get_path_type_fn::operator()(Message const& m) const
  {
    if constexpr(is_path_normalizable<std::decay_t<Message>>)
    {
      return decltype(hana::typeid_(get_normalized_path(m))){};
    }
    else
    {
      if constexpr(hana::is_a<create_path_tag, decltype(get_path(m))>)
      {
        using Key = typename std::decay_t<decltype(get_path(m))>::key;
        return hana::type_c<
          decltype(hana::append(get_path(m).parent_path, std::declval<Key>()))
        >;
      }
      else
      {
        return decltype(hana::typeid_(get_path(m))){};
      }
    }
  }
}

namespace boost::hana
{
  template <>
  struct equal_impl<nbdl::message::no_impl, nbdl::message::no_impl>
  {
    static constexpr hana::true_ apply(nbdl::message::no_impl
                                     , nbdl::message::no_impl)
    { return {}; };
  };

  template <>
  struct equal_impl<nbdl::message::create_path_tag
                  , nbdl::message::create_path_tag>
  {
    template <typename Key, typename ParentPath>
    static constexpr auto apply(nbdl::message::create_path<Key, ParentPath> const& x
                              , nbdl::message::create_path<Key, ParentPath> const& y)
    {
      return hana::equal(x.parent_path, y.parent_path);
    }
  };

#define NBDL_DETAIL_MESSAGE_TAG_OF_IMPL(NAME) \
  template <typename ...T> \
  struct tag_of<nbdl::message::NAME<T...>> \
  { using type = nbdl::message::NAME##_tag; }

  NBDL_DETAIL_MESSAGE_TAG_OF_IMPL(upstream_create);
  NBDL_DETAIL_MESSAGE_TAG_OF_IMPL(upstream_read);
  NBDL_DETAIL_MESSAGE_TAG_OF_IMPL(upstream_update);
  NBDL_DETAIL_MESSAGE_TAG_OF_IMPL(upstream_delete);

  NBDL_DETAIL_MESSAGE_TAG_OF_IMPL(downstream_create);
  NBDL_DETAIL_MESSAGE_TAG_OF_IMPL(downstream_read);
  NBDL_DETAIL_MESSAGE_TAG_OF_IMPL(downstream_update);
  NBDL_DETAIL_MESSAGE_TAG_OF_IMPL(downstream_delete);
}

namespace nbdl
{
#define NBDL_DETAIL_MESSAGE_BIND_SEQUENCE_IMPL(NAME) \
  template <> struct bind_sequence_impl<message::NAME##_tag> { \
    template <typename S, typename F> \
    static constexpr auto apply(S&& s, F&& f) \
    { return bind_sequence(std::forward<S>(s).storage, std::forward<F>(f)); } \
  } \

  NBDL_DETAIL_MESSAGE_BIND_SEQUENCE_IMPL(upstream_create);
  NBDL_DETAIL_MESSAGE_BIND_SEQUENCE_IMPL(upstream_read);
  NBDL_DETAIL_MESSAGE_BIND_SEQUENCE_IMPL(upstream_update);
  NBDL_DETAIL_MESSAGE_BIND_SEQUENCE_IMPL(upstream_delete);

  NBDL_DETAIL_MESSAGE_BIND_SEQUENCE_IMPL(downstream_create);
  NBDL_DETAIL_MESSAGE_BIND_SEQUENCE_IMPL(downstream_read);
  NBDL_DETAIL_MESSAGE_BIND_SEQUENCE_IMPL(downstream_update);
  NBDL_DETAIL_MESSAGE_BIND_SEQUENCE_IMPL(downstream_delete);

  // create_path

  template <>
  struct bind_sequence_impl<message::create_path_tag>
  {
    template <typename S, typename F>
    static constexpr auto apply(S&& s, F&& f)
    {
      return bind_sequence(std::forward<S>(s).parent_path, std::forward<F>(f));
    }
  };
}
#endif
