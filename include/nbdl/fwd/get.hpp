//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_GET_HPP
#define NBDL_FWD_GET_HPP

#include<boost/hana/core/when.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  template<typename T, typename = void>
  struct get_impl : get_impl<T, hana::when<true>> { };

  struct get_fn
  {
    template<typename Store>
    constexpr decltype(auto) operator()(Store&&) const;

    template<typename Store, typename Key>
    constexpr decltype(auto) operator()(Store&&, Key&&) const;
  };

  constexpr get_fn get{};
} // nbdl

#endif
