//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_BIND_MAP_HPP
#define NBDL_FWD_BIND_MAP_HPP

#include<boost/hana/core/when.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  template<typename T, typename = void>
  struct bind_map_impl : bind_map_impl<T, hana::when<true>> { };

  struct bind_map_fn
  {
    template <typename BindableMap, typename BindKeyValueFn>
    constexpr auto operator()(BindableMap&& s, BindKeyValueFn&& f) const;
  };

  constexpr bind_map_fn bind_map{};
} // nbdl

#endif


