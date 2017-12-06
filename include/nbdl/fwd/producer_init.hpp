//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_PRODUCER_INIT_HPP
#define NBDL_FWD_PRODUCER_INIT_HPP

#include <boost/hana/core/when.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  template <typename T, typename = void>
  struct producer_init_impl : producer_init_impl<T, hana::when<true>> { };

  //
  // A hook to allow producers to do initialization stuffs
  // and send messages after the context's components have
  // all been constructed.
  struct producer_init_fn
  {
    template <typename Producer>
    constexpr void operator()(Producer&) const;
  };

  constexpr producer_init_fn producer_init{};
}

#endif
