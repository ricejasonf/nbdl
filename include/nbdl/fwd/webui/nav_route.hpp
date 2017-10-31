//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_WEBUI_NAV_ROUTE_HPP
#define NBDL_FWD_WEBUI_NAV_ROUTE_HPP

#include <nbdl/context_store.hpp>
#include <nbdl/def/directives.hpp>
#include <nbdl/string.hpp>

#include <boost/hana/string.hpp>
#include <type_traits>

namespace nbdl::webui
{
  constexpr hana::string<'n', 'a', 'v', '_', 'r', 'o', 'u', 't', 'e'> nav_route_s{};

  // tag
  template <typename RouteMap>
  struct nav_route_producer { };

  // tag
  template <typename RouteMap>
  struct nav_route_store { };
}

namespace nbdl_def
{
  // NavRouteProducer directive
  constexpr auto NavRouteProducer = [](auto r_map)
  {
    using RouteMap = decltype(r_map);
    using nbdl::webui::nav_route_producer;
    using nbdl::webui::nav_route_store;
    using nav_route_s_t = std::decay_t<decltype(nbdl::webui::nav_route_s)>;

    return Producer(
      Type(nav_route_producer<RouteMap>{})
    , AccessPoint(
        PathKey<nav_route_s_t>
      , Store<nav_route_store<RouteMap>>
      , Actions(Update())
      , ListenPaths(Path<nav_route_s_t, hana::type<nbdl::string>>)
      , AccessPoint(
          PathKey<hana::type<nbdl::string>>
        , Store<nbdl::context_store<nbdl::string>>
        , Actions(Update())
        , ListenPaths(Path<nav_route_s_t>)
        )
      )
    );
  };
}

#endif
