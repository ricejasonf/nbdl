//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_NULL_STORE_HPP
#define NBDL_NULL_STORE_HPP

#include<nbdl/fwd/null_store.hpp>

namespace nbdl
{
  struct null_store
  {
    // stores nothing
  };

  template<>
  struct dispatch_impl<null_store>
  {
    template<typename Store, typename Message>
    static constexpr Message dispatch_fn(Store&&, Message&& m)
    {
      // always propagate message
      return std::forward<Message>(m);
    }
  };
} // nbdl

#endif
