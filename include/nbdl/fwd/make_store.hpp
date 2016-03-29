//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_MAKE_STORE_HPP
#define NBDL_FWD_MAKE_STORE_HPP

#include<boost/hana/core/when.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  template <typename T, typename = void>
  struct make_store_impl : make_store_impl<T, hana::when<true>> { };

  // TODO perhaps the types of Store, Path and Entity should
  // all be template params and not passed as values.
  // It may be simpler that way.
  template <typename T>
  struct make_store_fn
  {
    template <typename PathType>
    constexpr auto operator()(PathType const&) const;
  };

  template <typename T>
  constexpr make_store_fn<T> make_store{};
} // nbdl

#endif
