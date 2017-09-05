//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_WEBSOCKET_API_DETAIL_ENDPOINT_IMPL_HPP
#define NBDL_WEBSOCKET_API_DETAIL_ENDPOINT_IMPL_HPP

#include <nbdl/detail/js_val.hpp>
#include <nbdl/endpoint_handler.hpp>
#include <nbdl/websocket_api/detail/convert_payload.hpp>
#include <nbdl/websocket_api/detail/event_receiver.hpp>
#include <nbdl/websocket_api/json_payload.hpp>

#include <dyno.hpp>
#include <emscripten.h>
#include <memory>

namespace nbdl::websocket_api::detail
{
  namespace event = nbdl::endpoint_event;
  using js_val = nbdl::detail::js_val;

  template <typename Payload>
  void send_impl(js_val const& websocket, Payload const& p);

  struct endpoint_impl_tag { };

  template <typename Queue, typename Handler>
  struct endpoint_impl
  {
    using hana_tag = endpoint_impl_tag;
    using value_type = typename Queue::value_type;
    using Payload = value_type;

    template <typename E, typename Q, typename H>
    endpoint_impl(E const& e, Q&& q, H&& h)
      : receiver(event_receiver_impl<endpoint_impl>{*this})
      , send_queue(std::forward<Q>(q))
      , handler(std::forward<H>(h))
      , payload()
      , websocket()
      , event_data()
    {
      js_val::init_registry();
      connect(e.url);
    }

    endpoint_impl(endpoint_impl const&) = delete;

    void send_message(Payload const&);
    void send_close();

  private:

    friend event_receiver_impl<endpoint_impl>;

    template <typename String>
    void connect(String const&);

    void trigger(event::ready_t);
    void trigger(event::message_t);
    void trigger(event::close_t);

    bool is_connected();

    dyno::poly<EventReceiver> receiver;
    Queue send_queue;
    Handler handler;
    Payload payload;
    js_val websocket;
    js_val event_data;
  };

  extern "C"
  {
    void EMSCRIPTEN_KEEPALIVE _nbdl_endpoint_ready(void* ptr)
    {
      auto receiver = *static_cast<dyno::poly<EventReceiver>*>(ptr);
      receiver.virtual_(event::ready)(receiver);
    }

    void EMSCRIPTEN_KEEPALIVE _nbdl_endpoint_message(void* ptr)
    {
      auto receiver = *static_cast<dyno::poly<EventReceiver>*>(ptr);
      receiver.virtual_(event::message)(receiver);
    }

    void EMSCRIPTEN_KEEPALIVE _nbdl_endpoint_close(void* ptr)
    {
      auto receiver = *static_cast<dyno::poly<EventReceiver>*>(ptr);
      receiver.virtual_(event::close)(receiver);
    }

    int EMSCRIPTEN_KEEPALIVE _nbdl_endpoint_value_handle(void* ptr)
    {
      auto receiver = *static_cast<dyno::poly<EventReceiver>*>(ptr);
      return receiver.virtual_(value_handle_s)(receiver);
    }
  }

  template <typename Queue, typename Handler>
  template <typename String>
  void endpoint_impl<Queue, Handler>::connect(String const& str)
  {
    EM_ASM_(
      {
        var s = new window.WebSocket(Pointer_stringify($0, $1));
        s.binaryType = 'arraybuffer';

        s.addEventListener('open', function(event)
        {
          Module.__nbdl_endpoint_ready($2); 
        });

        // 'error' handler apparently needed in nodejs land
        // or it throws and exception
        s.addEventListener('error', function(event) { });

        s.addEventListener('message', function(event)
        {
          window.NBDL_DETAIL_JS_SET(
            Module.__nbdl_endpoint_value_handle($2)
          , event.data
          );
          Module.__nbdl_endpoint_message($2); 
        });

        s.addEventListener('close', function(event)
        {
          window.NBDL_DETAIL_JS_SET(
            Module.__nbdl_endpoint_value_handle($2)
          , event.code
          );
          Module.__nbdl_endpoint_close($2); 
        });

        window.NBDL_DETAIL_JS_SET($3, s);
      }
    , str.data()
    , str.size()
    , &receiver
    , websocket.handle()
    );
  }

  template <typename Queue, typename Handler>
  bool endpoint_impl<Queue, Handler>::is_connected()
  {
    int result = EM_ASM_INT(
      {
        // readyState 1 is OPEN
        return window.NBDL_DETAIL_JS_GET($0).readyState == 1;
      }
    , websocket.handle()
    );
    return result;
  }

  template <typename Queue, typename Handler>
  void endpoint_impl<Queue, Handler>::send_message(Payload const& p)
  {
    // TODO actually use the send_queue
    send_impl(websocket, p);
  }

  template <typename Queue, typename Handler>
  void endpoint_impl<Queue, Handler>::send_close()
  {
    EM_ASM_(
      {
        window.NBDL_DETAIL_JS_GET($0).close();
      }
    , websocket.handle()
    );
  }

  template <typename Queue, typename Handler>
  void endpoint_impl<Queue, Handler>::trigger(event::ready_t)
  {
    handler[event::ready](*this);  
  }

  template <typename Queue, typename Handler>
  void endpoint_impl<Queue, Handler>::trigger(event::message_t)
  {
    convert_payload<Payload>::apply(event_data, payload);
    handler[event::message](*this, payload);
  }

  template <typename Queue, typename Handler>
  void endpoint_impl<Queue, Handler>::trigger(event::close_t)
  {
    handler[event::close](*this);  
  }

  template <typename Payload>
  void send_impl(js_val const& websocket, Payload const& p)
  {
    if constexpr(hana::type_c<Payload> == hana::type_c<json_payload>)
    {
      EM_ASM_(
        {
          window.NBDL_DETAIL_JS_GET($0).send(JSON.stringify(
            window.NBDL_DETAIL_JS_GET($1)));
        }
      , websocket.handle()
      , p.handle()
      );
    }
    else
    {
      EM_ASM_(
        {
          window.NBDL_DETAIL_JS_GET($0).send(Module.HEAPU8.subarray($1, $2).buffer);
        }
      , websocket.handle()
      , p.data()
      , p.size()
      );
    }
  }
}

namespace nbdl
{
  // Endpoint

  template <>
  struct Endpoint<nbdl::websocket_api::detail::endpoint_impl_tag>
  {
    static constexpr bool value = true;
  };
}
#endif
