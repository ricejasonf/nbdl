//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_WEBSOCKET_CONNECTION_HPP
#define NBDL_WEBSOCKET_CONNECTION_HPP

#include <nbdl/async_enqueue.hpp>
#include <nbdl/catch.hpp>
#include <nbdl/concept/Endpoint.hpp>
#include <nbdl/endpoint_handler.hpp>
#include <nbdl/promise.hpp>
#include <nbdl/run_async_loop.hpp>
#include <nbdl/tap.hpp>
#include <nbdl/websocket/detail/message_reader.hpp>
#include <nbdl/websocket/detail/message_sender.hpp>
#include <nbdl/websocket/event.hpp>

#include <asio.hpp>
#include <boost/callable_traits/return_type.hpp>
#include <boost/hana/map.hpp>
#include <boost/mp11/utility.hpp>
#include <type_traits>

namespace nbdl::websocket::detail
{
  using asio::ip::tcp;
  namespace hana = boost::hana;

  enum struct control_frame_kind
  {
    NONE
  , CLOSE
  , PONG
  };

  template <typename Payload>
  struct control_frame
  {
    control_frame_kind kind = control_frame_kind::NONE;
    Payload payload = {};

    bool is_close()
    {
      return kind == control_frame_kind::CLOSE;
    }

    template <typename X>
    void to_close(X&& x)
    {
      kind    = control_frame_kind::CLOSE;
      payload = std::forward<X>(x);
    }

    template <typename X>
    void to_pong(X&& x)
    {
      if (kind != control_frame_kind::CLOSE)
      {
        kind    = control_frame_kind::PONG;
        payload = std::forward<X>(x);
      }
    }
  };
  
  struct endpoint_impl_tag { };

  template <typename Queue, typename Handler, typename SendMessageImpl, typename Derived_ = void>
  struct endpoint_impl
  {
    using hana_tag = endpoint_impl_tag;
    using value_type = typename Queue::value_type;
    using Payload = value_type;
    using Derived = boost::mp11::mp_if_c<std::is_void<Derived_>::value, endpoint_impl, Derived_>;

    inline Derived& self()
    {
      return static_cast<Derived&>(*this);
    }

    template <typename Q, typename H, typename InitFn>
    endpoint_impl(tcp::socket& s, Q&& queue_, H&& handler_, InitFn&& init)
      : socket(s)
      , pending_control_frame()
      , handler(std::forward<H>(handler_))
      , reader(detail::make_message_reader(
          hana::type_c<Payload>
        , socket
        , make_reader_handler(handler)
        ))
      , sender(socket)
      , send_queue(std::forward<Q>(queue_))
      , is_send_queue_running(false)
    {
      init(self(), socket, handler);
    }

    endpoint_impl(endpoint_impl const&) = delete;

    void send_message(Payload const&);
    void send_close();
    void _start_reading();

  private:
    void start_send_queue();
    bool is_sending_close();
    auto make_reader_handler(Handler&);

    tcp::socket& socket;
    detail::control_frame<Payload> pending_control_frame;
    Handler handler;

    using ReaderHandler = decltype(
      (std::declval<endpoint_impl>().*
       std::declval<decltype(&endpoint_impl::make_reader_handler)>())(handler)
    );

    detail::message_reader<Payload, ReaderHandler> reader;
    detail::message_sender<Payload> sender;
    Queue send_queue;
    bool is_send_queue_running = false;
  };

  template <typename Queue, typename Handler, typename SendMessageImpl, typename Derived>
  void endpoint_impl<Queue, Handler, SendMessageImpl, Derived>::send_message(Payload const& p)
  {
    nbdl::run_async(
      hana::make_tuple(
        nbdl::promise([this](auto& resolver, Payload const& payload)
        {
          nbdl::async_enqueue(resolver, send_queue, payload);
        })
      , [this]()
        {
          start_send_queue();
        }
      )
    , p
    );
  }

  template <typename Queue, typename Handler, typename SendMessageImpl, typename Derived>
  void endpoint_impl<Queue, Handler, SendMessageImpl, Derived>::send_close()
  {
    // TODO Use error codes defined in
    // RFC 6455 7.4.1
    pending_control_frame.to_close(Payload{});
    start_send_queue();
  }

  template <typename Queue, typename Handler, typename SendMessageImpl, typename Derived>
  void endpoint_impl<Queue, Handler, SendMessageImpl, Derived>::_start_reading()
  {
    // Calling this function twice is UB
    reader.keep_reading();
  }

  template <typename Queue, typename Handler, typename SendMessageImpl, typename Derived>
  void endpoint_impl<Queue, Handler, SendMessageImpl, Derived>::start_send_queue()
  {
    if (is_send_queue_running)
    {
      return;
    }
    is_send_queue_running = true;
    // run_async_loop until queue is empty
    nbdl::run_async_loop(hana::make_tuple(
      nbdl::promise([this](auto& resolver)
      {
        if (is_sending_close())
        {
          sender.data.payload = pending_control_frame.payload;
          nbdl::run_async(hana::make_tuple(
            nbdl::promise([&](auto& resolver_)
            {
              detail::send_close(resolver_, sender);
              // keep is_send_queue_running == true to prevent more messages
              // after sending the close frame
              resolver.terminate();
            })
          , nbdl::catch_([&](std::error_code error_code)
            {
              resolver.reject(error_code);
            })
          ));
        }
        else
        {
          // keep sending
          resolver.resolve();
        }
      })
    , nbdl::promise([this](auto& resolver)
      {
        if (send_queue.size() > 0)
        {
          sender.data.payload = send_queue.front();
          SendMessageImpl{}(resolver, sender);
        }
        else
        {
          is_send_queue_running = false;
          resolver.terminate();
        }
      })
    , [this](auto&& ...)
      {
        if (send_queue.size() > 0)
        {
          // pop after the message was sent without error
          send_queue.pop();
        }
      }
    , nbdl::catch_([this](std::error_code error)
      {
        is_send_queue_running = false;
        if ( is_sending_close() 
         && (asio::error::eof == error || asio::error::connection_reset == error)
           )
        {
          //expected socket close
          handler[endpoint_event::close]();
        }
        else
        {
          handler[hana::type_c<std::error_code>](error);
        }
      })
    , nbdl::catch_([this](auto&& error)
      {
        is_send_queue_running = false;
        handler[hana::typeid_(error)](std::forward<decltype(error)>(error));
      })
    ));
  }

  template <typename Queue, typename Handler, typename SendMessageImpl, typename Derived>
  bool endpoint_impl<Queue, Handler, SendMessageImpl, Derived>::is_sending_close()
  {
    return pending_control_frame.is_close();
  }

  template <typename Queue, typename Handler, typename SendMessageImpl, typename Derived>
  auto endpoint_impl<Queue, Handler, SendMessageImpl, Derived>::make_reader_handler(Handler&)
  {
    namespace read_event = detail::read_event;

    return hana::make_map(
      hana::make_pair(read_event::message, [this](auto, auto& frame_ctx)
      {
        // move from the payload object in the reader
        //handler[endpoint_event::message](self(), std::move(frame_ctx.payload));
        handler[endpoint_event::message](self(), frame_ctx.payload);
      })
    , hana::make_pair(read_event::close, [this](auto, auto const&)
      {
        // TODO the handler should get some info on why it was closed
        if (is_sending_close())
        {
          handler[endpoint_event::close](self());
        }
        else
        {
          send_close();
        }
      })
    , hana::make_pair(read_event::ping, [this](auto, auto const& frame_ctx)
      {
        pending_control_frame.to_pong(std::move(frame_ctx.payload));
      })
    , hana::make_pair(read_event::pong, [](auto&& ...) { })
    , hana::make_pair(read_event::bad_input, [this](auto&& ...)
      {
        handler[event::bad_request](self());
      })
    , hana::make_pair(hana::type_c<std::error_code>, [this](std::error_code error, auto const&)
      {
        handler[hana::type_c<std::error_code>](self(), error);
      })
    );
  }
}

namespace nbdl
{
  // Endpoint

  template <>
  struct Endpoint<nbdl::websocket::detail::endpoint_impl_tag>
  {
    static constexpr bool value = true;
  };
}

#endif
