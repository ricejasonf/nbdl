//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_BIND_STRING_HPP
#define NBDL_FWD_BIND_STRING_HPP

#include<boost/hana/core/when.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  template<typename T, typename = void>
  struct bind_string_impl : bind_string_impl<T, hana::when<true>> { };

  struct bind_string_fn
  {
    template<typename BindableString, typename BindFn>
    constexpr auto operator()(BindableString&&, BindFn&&) const;
  };

  constexpr bind_string_fn bind_string{};
}

#endif
