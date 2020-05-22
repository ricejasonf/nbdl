//
// Copyright Jason Rice 2019
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MPDEF_UTILITY_HPP
#define NBDL_MPDEF_UTILITY_HPP

#include <boost/hana.hpp>

#include <type_traits>
#include <utility>

namespace mpdef
{
  namespace hana = boost::hana;

  // idempotently converts to a hana Constant or string
  heavy_macro to_constant(x) = [] {
    using T = std::decay_t<decltype(x)>;
    if constexpr (std::is_integral_v<T>)
      return hana::integral_constant<T, x>{};
    else if constexpr (std::is_same_v<char const*, T>)
      return BOOST_HANA_STRING(x);
    else return x;
  }();
}
#endif
