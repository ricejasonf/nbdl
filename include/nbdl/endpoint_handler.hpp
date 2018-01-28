//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_ENDPOINT_HANDLER_HPP
#define NBDL_ENDPOINT_HANDLER_HPP

#include <boost/hana/map.hpp>
#include <boost/hana/pair.hpp>
#include <boost/hana/type.hpp>
#include <string>
#include <utility>

namespace nbdl
{
  namespace hana = boost::hana;

  namespace endpoint_event
  {
    struct ready_t { };
    struct message_t { };
    struct close_t { };

    // keys
    constexpr hana::type<ready_t>    ready{};
    constexpr hana::type<message_t>  message{};
    constexpr hana::type<close_t>    close{};

    // constructs a pair
    template <typename T>
    struct on_t
    {
      template <typename Fn>
      constexpr auto operator()(Fn&& fn) const
      { return hana::make_pair(hana::type<T>{}, std::forward<Fn>(fn)); };

      template <typename Fn>
      constexpr auto operator=(Fn&& fn) const
      { return hana::make_pair(hana::type<T>{}, std::forward<Fn>(fn)); };
    };

    template <typename T>
    constexpr auto on         = on_t<T>{};
    constexpr auto on_ready   = on<ready_t>;
    constexpr auto on_message = on<message_t>;
    constexpr auto on_close   = on<close_t>;
  }

  constexpr auto endpoint_handler = [](auto&& ...pairs)
  {
    return hana::make_map(std::forward<decltype(pairs)>(pairs)...);
  };
}

#endif
