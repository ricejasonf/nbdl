//
// Copyright Jason Rice 2019
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_WEBUI_SET_ROUTE_HPP
#define NBDL_WEBUI_SET_ROUTE_HPP

#include <nbdl/message.hpp>

namespace nbdl::webui
{
  constexpr auto nav_route_path = hana::tuple<
    hana::string<'n', 'a', 'v', '_', 'r', 'o', 'u', 't', 'e'>
  >{};

  template <typename Route, typename Variant>
  struct set_route_rt
  {
    // Support DefaultConstructible for stateless routes.
    template <typename Send>
    void operator()(Send send) const
    {
      send(nbdl::message::make_upstream_update(
        nav_route_path
      , nbdl::message::no_uid
      , Variant(Route{})
      ));
    }

    template <typename Send>
    void operator()(Send send, Route const& route) const
    {
      send(nbdl::message::make_upstream_update(
        nav_route_path
      , nbdl::message::no_uid
      , Variant(route)
      ));
    }
  };

  template <typename RouteMap>
  struct set_route_fn
  {
    static using operator()(using auto route_name)
    {
      return set_route_rt<
        typename decltype(RouteMap::get_type(route_name))::type
      , typename RouteMap::variant
      >{};
    }
  };

  template <typename RouteMap>
  constexpr auto make_set_route(RouteMap)
  { return set_route_fn<RouteMap>{}; }
}

#endif
