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
#include <memory>
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

  template <typename Derived, typename RawImpl>
  using coalesce_derived_t = boost::mp11::mp_if_c<std::is_void<Derived>::value, RawImpl, Derived>;
  
  struct endpoint_impl_tag { };

  template <typename Queue, typename Handler, typename SendMessageImpl, typename Derived_ = void>
  struct endpoint_impl
    : std::enable_shared_from_this<coalesce_derived_t<Derived_, endpoint_impl<Queue
                                                                            , Handler
                                                                            , SendMessageImpl
                                                                            , Derived_>>>
  {
    using hana_tag = endpoint_impl_tag;
    using value_type = typename Queue::value_type;
    using Payload = value_type;
    using Derived = coalesce_derived_t<Derived_, endpoint_impl>;

    inline Derived& self()
    {
      return static_cast<Derived&>(*this);
    }

    template <typename Q, typename H, typename InitFn>
    endpoint_impl(tcp::socket&& s, Q&& queue_, H&& handler_, InitFn&& init)
      : socket(std::move(s))
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
      , is_started(false)
      , is_stopped(false)
      , is_close_received(false)
    {
      init(self(), socket, handler);
    }

    endpoint_impl(endpoint_impl const&) = delete;

    void send_message(Payload const&);
    void send_close();
    void start_messaging();
    bool stop();

  private:
    void start_send_queue();
    bool is_sending_close();
    void mark_is_close_received();
    auto make_reader_handler(Handler&);

    template <typename Key, typename ...Args>
    void call_handler(Key, Args&& ...);

    tcp::socket socket;
    detail::control_frame<Payload> pending_control_frame;
    Handler handler;

    using ReaderHandler = decltype(
      (std::declval<endpoint_impl>().*
       std::declval<decltype(&endpoint_impl::make_reader_handler)>())(handler)
    );

    detail::message_reader<Payload, ReaderHandler> reader;
    detail::message_sender<Payload> sender;
    Queue send_queue;
    bool is_send_queue_running : 1;
    bool is_started : 1;
    bool is_stopped : 1;
    bool is_close_received : 1;
  };

  template <typename Queue, typename Handler, typename SendMessageImpl, typename Derived>
  void endpoint_impl<Queue, Handler, SendMessageImpl, Derived>::send_message(Payload const& p)
  {
    auto shared_this = this->shared_from_this();

    nbdl::run_async(
      hana::make_tuple(
        nbdl::promise([this, shared_this](auto& resolver, Payload const& payload)
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
  void endpoint_impl<Queue, Handler, SendMessageImpl, Derived>::start_messaging()
  {
    if (not is_started && not is_stopped)
    {
      is_started = true;

      reader.keep_reading(this->shared_from_this());

      if (send_queue.size() > 0)
      {
        start_send_queue();
      }
    }
  }

  template <typename Queue, typename Handler, typename SendMessageImpl, typename Derived>
  void endpoint_impl<Queue, Handler, SendMessageImpl, Derived>::start_send_queue()
  {
    if (       is_send_queue_running
            or is_stopped
        or not is_started)
    {
      return;
    }

    auto shared_this = this->shared_from_this();
    is_send_queue_running = true;

    // run_async_loop until queue is empty
    nbdl::run_async_loop(hana::make_tuple(
      nbdl::promise([this, shared_this](auto& resolver)
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
          , [&](auto& /*sender*/)
            {
              if (is_close_received)
              {
                call_handler(endpoint_event::close);
              }
            }
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
          call_handler(endpoint_event::close);
        }
        else
        {
          call_handler(hana::type_c<std::error_code>, error);
        }
      })
    , nbdl::catch_([this](auto&& error)
      {
        is_send_queue_running = false;
        call_handler(hana::typeid_(error), std::forward<decltype(error)>(error));
      })
    ));
  }

  template <typename Queue, typename Handler, typename SendMessageImpl, typename Derived>
  bool endpoint_impl<Queue, Handler, SendMessageImpl, Derived>::is_sending_close()
  {
    return pending_control_frame.is_close();
  }

  template <typename Queue, typename Handler, typename SendMessageImpl, typename Derived>
  void endpoint_impl<Queue, Handler, SendMessageImpl, Derived>::mark_is_close_received()
  {
    is_close_received = true;
  }

  template <typename Queue, typename Handler, typename SendMessageImpl, typename Derived>
  auto endpoint_impl<Queue, Handler, SendMessageImpl, Derived>::make_reader_handler(Handler&)
  {
    namespace read_event = detail::read_event;

    return hana::make_map(
      hana::make_pair(read_event::message, [this](auto, auto& frame_ctx)
      {
        // move from the payload object in the reader
        call_handler(endpoint_event::message, frame_ctx.payload);
      })
    , hana::make_pair(read_event::close, [this](auto, auto const&)
      {
        mark_is_close_received();

        if (is_sending_close())
        {
          call_handler(endpoint_event::close);
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
        call_handler(event::bad_request);
      })
    , hana::make_pair(hana::type_c<std::error_code>, [this](std::error_code error, auto const&)
      {
        call_handler(hana::type_c<std::error_code>, error);
      })
    );
  }

  template <typename Queue, typename Handler, typename SendMessageImpl, typename Derived>
  template <typename Key, typename ...Args>
  void endpoint_impl<Queue, Handler, SendMessageImpl, Derived>::call_handler(Key key, Args&& ...args)
  {
    if (is_stopped)
    {
      return;
    }

    handler[key](self(), std::forward<Args>(args)...);
  }

  template <typename Queue, typename Handler, typename SendMessageImpl, typename Derived>
  bool endpoint_impl<Queue, Handler, SendMessageImpl, Derived>::stop()
  {
    is_stopped = true;
    try {
      socket.shutdown(tcp::socket::shutdown_both);
    } catch(...) {
      return false;
    }
    return true;
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
