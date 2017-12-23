//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_WEBUI_DETAIL_DOM_EVENT_LISTENER_HPP
#define NBDL_WEBUI_DETAIL_DOM_EVENT_LISTENER_HPP

#include <nbdl/webui/detail/event_receiver.hpp>

#include <emscripten.h>
#include <memory>
#include <type_traits>
#include <utility>

namespace nbdl::webui::detail
{
  class dom_event_listener
  {
    std::unique_ptr<event_receiver> receiver;
    emscripten::val handler;
    emscripten::val el;
    emscripten::val event_name;

    inline void init()
    {
      el.template call<void>(
        "addEventListener"
      , event_name
      , handler
      );
    }

    public:

    template <typename ReceiverImpl, typename ValElement>
    dom_event_listener(ReceiverImpl&& receiver_impl
                     , ValElement&& el_
                     , emscripten::val&& event_name_)
      : receiver(make_event_receiver(std::forward<ReceiverImpl>(receiver_impl)))
      , handler(receiver->virtual_(handler_s)(*receiver).as_val())
      , el(std::forward<ValElement>(el_))
      , event_name(std::move(event_name_))
    {
      init();
    }

    dom_event_listener(emscripten::val el_
                     , emscripten::val&& event_name_
                     , emscripten::val&& handler_)
      : receiver()
      , handler(std::move(handler_))
      , el(std::move(el_))
      , event_name(std::move(event_name_))
    {
      init();
    }

    dom_event_listener(dom_event_listener const&) = delete;

    ~dom_event_listener()
    {
      el.template call<void>(
        "removeEventListener"
      , event_name
      , handler
      );
    }
  };

  template <typename ReceiverImpl, typename ValElement>
  inline std::unique_ptr<dom_event_listener> make_dom_event_listener(ReceiverImpl&& receiver_impl
                                                                   , ValElement&& el
                                                                   , emscripten::val&& event_name)
  {
    return std::unique_ptr<dom_event_listener>(
      new dom_event_listener(
        std::forward<ReceiverImpl>(receiver_impl)
      , std::forward<ValElement>(el)
      , std::move(event_name)
      )
    );
  }

  inline std::unique_ptr<dom_event_listener> make_dom_event_listener(emscripten::val const& el
                                                                   , emscripten::val&& event_name
                                                                   , emscripten::val&& handler)
  {
    return std::unique_ptr<dom_event_listener>(
      new dom_event_listener(
        el
      , std::move(event_name)
      , std::move(handler)
      )
    );
  }
}

#endif
