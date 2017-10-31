//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_WEB_NAV_ROUTE_HPP
#define NBDL_WEB_NAV_ROUTE_HPP

#include <nbdl/fwd/webui/nav_route.hpp>
#include <nbdl/message.hpp>
#include <nbdl/string.hpp>
#include <nbdl/variant.hpp>
#include <nbdl/webui/detail/event_receiver.hpp>
#include <nbdl/webui/route_map.hpp>

#include <emscripten.h>
#include <type_traits>
#include <utility>

namespace nbdl::webui
{
  namespace hana = boost::hana;

  namespace detail
  {
    template <typename Message>
    constexpr bool is_nav_route_path = decltype(hana::equal(
      message::get_path_type(std::declval<Message>())
    , hana::typeid_(hana::make_tuple(nbdl::webui::nav_route_s))
    ))::value;

    template <typename Message>
    constexpr bool is_nav_route_string_path = decltype(hana::equal(
      message::get_path_type(std::declval<Message>())
    , hana::typeid_(hana::make_tuple(nbdl::webui::nav_route_s, hana::type_c<nbdl::string>))
    ))::value;

    inline void web_route_update(nbdl::string const& route)
    {
      EM_ASM_(
        {
          window.history.pushState(null, null, Pointer_stringify($0));
        }
      , route.data()
      );
    }

    template <typename Context>
    struct popstate_event_receiver
    {
      Context context;
      std::unique_ptr<event_receiver_vals> vals;

      popstate_event_receiver(Context c)
        : context(c)
        , vals(new event_receiver_vals{})
      {
        EM_ASM_(
          {
            window.onpopstate = function()
            {
              // call the handler
              Module.NBDL_DETAIL_JS_GET($0)();
            };
          }
        , vals->handler.handle()
        );
      }

      void receive_event()
      {
        std::size_t length = EM_ASM_INT({ return window.location.pathname.length; }, 0);
        nbdl::string route(length, '\0');

        EM_ASM_(
          {
            stringToAscii(window.location.pathname, $0);
          }
        , route.data()
        );

        context.push(
          message::make_downstream_update(
            hana::make_tuple(nav_route_s, hana::type_c<nbdl::string>)
          , message::no_uid
          , route
          , message::no_is_confirmed
          )
        );
      }
    };
  }

  template <typename Context, typename RouteMap>
  struct nav_route_producer_impl
  {
    using hana_tag = nav_route_producer<RouteMap>;
    using route_map = RouteMap;

    Context context;
    std::unique_ptr<detail::event_receiver> receiver;

    nav_route_producer_impl(Context c)
      : context(c)
      , receiver(detail::make_event_receiver(detail::popstate_event_receiver<Context>{c}))
    {
      // load the store with the initial value
      receiver->virtual_(detail::receive_event_s)(*receiver);
    }
  };

  template <typename RouteMap>
  struct nav_route_store_impl
  {
    using hana_tag = nav_route_store<RouteMap>;

    typename RouteMap::variant value{};
  };
}

namespace nbdl
{
  // nav_route_producer

  template <typename RouteMap>
  struct make_producer_impl<nbdl::webui::nav_route_producer<RouteMap>>
  {
    template <typename Context, typename ...Args>
    static nbdl::webui::nav_route_producer_impl<Context, RouteMap> apply(Context c, Args&&...)
    { return {c}; }
  };

  template <typename RouteMap>
  struct send_upstream_message_impl<nbdl::webui::nav_route_producer<RouteMap>>
  {
    template <typename Producer, typename Message>
    static void apply(Producer&, Message&& m)
    {
      static_assert(message::is_update<Message>);
      if constexpr(nbdl::webui::detail::is_nav_route_string_path<Message>)
      {
        webui::detail::web_route_update(message::get_payload(m));
      }
      else if constexpr(nbdl::webui::detail::is_nav_route_path<Message>)
      {
        webui::detail::web_route_update(RouteMap{}.to_string(message::get_payload(m)));
      }
    }
  };

  // nav_route_store

  template <typename RouteMap>
  struct apply_foreign_message_impl<nbdl::webui::nav_route_store<RouteMap>>
  {
    template <typename Message, typename Fn>
    static void apply(nbdl::webui::nav_route_store_impl<RouteMap>& store, Message&& m, Fn&& f)
    {
      if constexpr(
            message::is_update<Message>
        and message::is_downstream<Message>
        and webui::detail::is_nav_route_string_path<Message>
      )
      {
        constexpr RouteMap map{};

        store.value = map.from_string(message::get_payload(std::forward<Message>(m)));

        std::forward<Fn>(f)(hana::make_tuple(webui::nav_route_s));
      }
    }
  };

  template <typename RouteMap>
  struct make_store_impl<nbdl::webui::nav_route_store<RouteMap>>
  {
    static auto apply(...)
      -> nbdl::webui::nav_route_store_impl<RouteMap>
    { return {}; }
  };

  template <typename RouteMap>
  struct match_impl<nbdl::webui::nav_route_store<RouteMap>>
  {
    template <typename Store, typename Key, typename ...Args>
    static void apply(Store&& s, Key&&, Args&& ...args)
    {
      nbdl::match(
        std::forward<Store>(s).value
      , std::forward<Args>(args)...
      );
    }
  };
}

#endif
