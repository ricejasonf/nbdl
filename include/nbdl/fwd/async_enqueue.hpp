//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_ASYNC_ENQUEUE_HPP
#define NBDL_FWD_ASYNC_ENQUEUE_HPP

#include<boost/hana/core/when.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  // async_enqueue is a customization point to allow persistent
  // queues to be used with a connection to an Endpoint
  template<typename T, typename = void>
  struct async_enqueue_impl : async_enqueue_impl<T, hana::when<true>> { };

  struct async_enqueue_fn
  {
    template <typename Resolver, typename Queue, typename Message>
    void operator()(Resolver&, Queue&, Message&&) const;
  };

  constexpr async_enqueue_fn async_enqueue{};
}

#endif
