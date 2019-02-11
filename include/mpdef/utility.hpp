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

  template <bool>
  struct constexpr_if_;

  template <>
  struct constexpr_if_<true>
  {
    static using apply(using auto x, using auto y)
    {
      return x;
    }
  };

  template <>
  struct constexpr_if_<false>
  {
    static using apply(using auto x, using auto y)
    {
      return y;
    }
  };

  using constexpr_if(using auto cond, using auto x, using auto y)
  {
    return constexpr_if_<cond>(x, y);
  }

  template <typename>
  struct iota_;

  template <auto ...i>
  struct iota_<std::index_sequence<i...>>
  {
    static using apply() {
      return i;
    }
  };

  using iota(using auto N)
  {
    return iota_<std::make_index_sequence<N>>::apply();
  }

  template <typename, typename = void>
  struct to_constant_
  {
    static using apply(using auto x)
    {
      return x;
    }
  };

  template <typename T>
  struct to_constant_<T, std::enable_if_t<std::is_integral_v<T>>>
  {
    static using apply(using auto x)
    {
      return hana::integral_constant<std::decay_t<T>, x>{};
    }
  };

  template <typename>
  struct _hana_string_helper;

  template <auto ...i>
  struct _hana_string_helper<std::index_sequence<i...>> {
    static using apply(using auto x)
    {
      return hana::string<x[i]...>{};
    }
  };

  template <std::size_t N>
  struct to_constant_<char[N]>
  {
    static using apply(using auto x)
    {
      return _hana_string_helper<std::make_index_sequence<N - 1>>::apply(x);
    }
#if 0 // causes ICE
    static using apply(using auto ...x)
    {
      using foo = typename hana::string<x[iota(N - 1)]...>::foo;
      return hana::string<x[iota(N - 1)]...>{};
    }
#endif
  };

  // idempotently converts to a hana Constant or string
  using to_constant(using auto x)
  {
    return to_constant_<hana::tag_of_t<decltype(x)>>::apply(x);
  }
}
#endif
