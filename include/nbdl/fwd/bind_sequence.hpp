//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_BIND_SEQUENCE_HPP
#define NBDL_FWD_BIND_SEQUENCE_HPP

#include<boost/hana/core/when.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  template<typename T, typename = void>
  struct bind_sequence_impl : bind_sequence_impl<T, hana::when<true>> { };

  struct bind_sequence_fn
  {
    template<typename BindableSequence, typename BindFn>
    constexpr void operator()(BindableSequence&&, BindFn&&) const;
  };

  constexpr bind_sequence_fn bind_sequence{};
} // nbdl

#endif
