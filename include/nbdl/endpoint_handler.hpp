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

    constexpr hana::type<ready_t>    ready{};
    constexpr hana::type<message_t>  message{};
    constexpr hana::type<close_t>    close{};
  }

  constexpr auto endpoint_handler = [](auto&& ...pairs)
  {
    return hana::make_map(std::forward<decltype(pairs)>(pairs)...);
  };
}

#endif
