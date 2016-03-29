//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_HAS_HPP
#define NBDL_FWD_HAS_HPP

#include<boost/hana/core/when.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  template<typename T, typename = void>
  struct has_impl : has_impl<T, hana::when<true>> { };

  struct has_fn
  {
    template<typename Store, typename Path>
    constexpr bool operator()(Store&&, Path&&) const;
  };

  constexpr has_fn has{};
} // nbdl

#endif

