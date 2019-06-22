//
// Copyright Jason Rice 2019
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_CONSUMER_INIT_HPP
#define NBDL_FWD_CONSUMER_INIT_HPP

#include <boost/hana/core/when.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  template <typename T, typename = void>
  struct consumer_init_impl : consumer_init_impl<T, hana::when<true>> { };

  //
  // A hook to allow consumers to do initialization stuffs
  // and send messages after the context's components have
  // all been constructed.
  struct consumer_init_fn
  {
    template <typename Consumer>
    constexpr void operator()(Consumer&) const;
  };

  constexpr consumer_init_fn consumer_init{};
}

#endif
