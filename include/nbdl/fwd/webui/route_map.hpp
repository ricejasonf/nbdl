//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_WEBUI_ROUTE_MAP_HPP
#define NBDL_FWD_WEBUI_ROUTE_MAP_HPP

#include <mpdef/pair.hpp>
#include <mpdef/utility.hpp>
#include <nbdl/variant.hpp>

namespace nbdl::webui
{
  template <typename ...>
  class route_map;

  template <typename T>
  struct route_pair_fn
  {
    static using operator()(using auto name)
    {
      return mpdef::pair<decltype(mpdef::to_constant(name)), T>{};
    }
  };

  template <>
  struct route_pair_fn<void>
  {
    static using operator()(using auto name)
    {
      return mpdef::pair<
        decltype(mpdef::to_constant(name))
      , decltype(mpdef::to_constant(name))
      >{};
    }
  };

  template <typename T = void>
  constexpr route_pair_fn<T> route_pair{};


  struct make_route_map_fn
  {
    template <typename ...RoutePairs>
    constexpr auto operator()(RoutePairs ...) const;
  };

  constexpr make_route_map_fn make_route_map{};

  struct route_not_found { };

  template <typename... Tn>
  using route_variant = ::nbdl::detail::variant<route_not_found, Tn...>;
}

#endif
