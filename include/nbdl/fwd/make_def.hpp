//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_MAKE_DEF_HPP
#define NBDL_FWD_MAKE_DEF_HPP

#include<boost/hana/core/when.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  template <typename T, typename = void>
  struct make_def_impl : make_def_impl<T, hana::when<true>> { };

  struct make_def_fn
  {
    template <typename T>
    constexpr auto operator()(T) const;
  };

  constexpr make_def_fn make_def{};
} // nbdl

#endif
