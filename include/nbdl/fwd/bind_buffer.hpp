//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_BIND_BUFFER_HPP
#define NBDL_FWD_BIND_BUFFER_HPP

#include<boost/hana/core/when.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  template<typename T, typename = void>
  struct bind_buffer_impl : bind_buffer_impl<T, hana::when<true>> { };

  struct bind_buffer_fn
  {
    template<typename BindableBuffer, typename BindFn>
    constexpr auto operator()(BindableBuffer&&, BindFn&&) const;
  };

  constexpr bind_buffer_fn bind_buffer{};
}

#endif
