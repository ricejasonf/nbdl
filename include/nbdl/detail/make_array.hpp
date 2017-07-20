//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DETAIL_MAKE_ARRAY_HPP
#define NBDL_DETAIL_MAKE_ARRAY_HPP

#include<array>
#include<utility>

namespace nbdl::detail
{
  template <typename D = void>
  constexpr auto make_array = [](auto&& ...x)
  {
    return std::array<D, sizeof...(x)>{{
      std::forward<decltype(x)>(x)...
    }};
  };

  template <>
  constexpr auto make_array<void> = [](auto&& ...x)
  {
    using T = std::decay_t<std::common_type_t<decltype(x)...>>;
    return std::array<T, sizeof...(x)>{{
      std::forward<decltype(x)>(x)...
    }};
  };
}

#endif
