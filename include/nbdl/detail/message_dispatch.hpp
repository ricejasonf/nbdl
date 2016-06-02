//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DETAIL_MESSAGE_DISPATCHER_HPP
#define NBDL_DETAIL_MESSAGE_DISPATCHER_HPP

#include <nbdl/message.hpp>
#include <nbdl/fwd/make_store.hpp> // not_found

#include <boost/hana/core/when.hpp>
#include <type_traits>

// message_dispatch relies on the
// message's representation as a tuple
// for the sake of compile-time performance.

#define NBDL_DETAIL_FORWARD_HANDLE_MESSAGE_BLOCK(METHOD) { \
  template <typename Handler, typename Message, typename ...Args> \
  static constexpr decltype(auto) apply(Handler&& h, Message&& m, Args&& ...args) \
  { return std::forward<Handler>(h).METHOD(std::forward<Message>(m), std::forward<Args>(args)...); } }

namespace nbdl { namespace detail
{
  namespace hana = boost::hana;

  template <typename Message, typename = void>
  struct message_dispatch_helper;

  template <typename Path, typename Message, typename = void>
  struct message_dispatch;

  template <typename Path, typename Message_>
  struct message_dispatch<Path, Message_, std::enable_if_t<
      decltype(!message::is_foreign<Path>(std::declval<Message_>()))::value
    >
  >
  {
    template <typename Handler, typename Message, typename ...Args>
    static constexpr decltype(auto) apply(Handler&& h, Message&& m, Args&& ...args)
    {
      return message_dispatch_helper<Message>::apply(
        std::forward<Handler>(h),
        std::forward<Message>(m),
        std::forward<Args>(args)...
      ); 
    }
  };

  template <typename Path, typename Message_>
  struct message_dispatch<Path, Message_, std::enable_if_t<
      decltype(message::is_foreign<Path>(std::declval<Message_>()))::value
    >
  > NBDL_DETAIL_FORWARD_HANDLE_MESSAGE_BLOCK(handle_foreign);

  template <typename ...T>
  struct message_dispatch_helper<
    hana::tuple<
      message::channel::upstream,
      message::action::read,
      T...
    >
  > NBDL_DETAIL_FORWARD_HANDLE_MESSAGE_BLOCK(handle_upstream_read);

  template <typename T1, typename T2, typename T3, typename Payload, typename ...T>
  struct message_dispatch_helper<
    hana::tuple<
      message::channel::downstream,
      message::action::read,
      T1, T2, T3,
      hana::optional<Payload>,
      T...
    >
  > NBDL_DETAIL_FORWARD_HANDLE_MESSAGE_BLOCK(handle_downstream_read);

  template <typename T1, typename T2, typename T3, typename ...T>
  struct message_dispatch_helper<
    hana::tuple<
      message::channel::downstream,
      message::action::read,
      T1, T2, T3,
      hana::optional<nbdl::not_found>,
      T...
    >
  > NBDL_DETAIL_FORWARD_HANDLE_MESSAGE_BLOCK(handle_downstream_read_not_found);

  template <typename ...T>
  struct message_dispatch_helper<
    hana::tuple<
      message::channel::upstream,
      message::action::create,
      T...
    >
  > NBDL_DETAIL_FORWARD_HANDLE_MESSAGE_BLOCK(handle_upstream_create);

  template <typename ...T>
  struct message_dispatch_helper<
    hana::tuple<
      message::channel::downstream,
      message::action::create,
      T...
    >
  > NBDL_DETAIL_FORWARD_HANDLE_MESSAGE_BLOCK(handle_downstream_create);

  template <typename ...T>
  struct message_dispatch_helper<
    hana::tuple<
      message::channel::upstream,
      message::action::update_raw,
      T...
    >
  > NBDL_DETAIL_FORWARD_HANDLE_MESSAGE_BLOCK(handle_upstream_update_raw);

  template <typename ...T>
  struct message_dispatch_helper<
    hana::tuple<
      message::channel::downstream,
      message::action::update_raw,
      T...
    >
  > NBDL_DETAIL_FORWARD_HANDLE_MESSAGE_BLOCK(handle_downstream_update_raw);

  template <typename ...T>
  struct message_dispatch_helper<
    hana::tuple<
      message::channel::upstream,
      message::action::delete_,
      T...
    >
  > NBDL_DETAIL_FORWARD_HANDLE_MESSAGE_BLOCK(handle_upstream_delete);

  template <typename ...T>
  struct message_dispatch_helper<
    hana::tuple<
      message::channel::downstream,
      message::action::delete_,
      T...
    >
  > NBDL_DETAIL_FORWARD_HANDLE_MESSAGE_BLOCK(handle_downstream_delete);

}} // nbdl::detail

#endif
