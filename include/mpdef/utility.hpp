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

namespace mpdef {
  namespace hana = boost::hana;

  // works around clang bug reported on llvm bugs #20033
  heavy_macro convert_string(s, ...i) = [] {
    constexpr auto len = sizeof(s) - 1;
    if constexpr(len > sizeof...(i))
      return convert_string(s, i..., sizeof...(i));
    else
      return hana::string<s[i]...>{};
  }();

  // idempotently converts to a hana Constant or string
  heavy_macro to_constant(x) = [=](auto inst) {
    using T = std::decay_t<decltype(inst)>;

    if constexpr (std::is_integral_v<T>)
      return std::integral_constant<T, x>{};
    else if constexpr (std::is_same_v<const char*, T>)
      return convert_string(x);
    else return inst;
  }(x);

  static_assert(convert_string("foo") == hana::string<'f', 'o', 'o'>{});
}

#endif
