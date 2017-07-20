//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_ASYNC_ENQUEUE_HPP
#define NBDL_ASYNC_ENQUEUE_HPP

#include <nbdl/fwd/async_enqueue.hpp>

namespace nbdl
{
  template <typename Resolver, typename Queue, typename Message>
  void async_enqueue_fn::operator()(Resolver& resolver
                               , Queue& queue
                               , Message&& message) const
  {
    using Tag = hana::tag_of_t<Queue>;
    using Impl = async_enqueue_impl<Tag>;

    // TODO check concept Resolver

    Impl::apply(resolver, queue, std::forward<Message>(message));
  };

  // The default behaviour is to assume a
  // synchronous operation and resolve
  template <typename Tag, bool condition>
  struct async_enqueue_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    template <typename Resolver, typename Queue, typename Message>
    static void apply(Resolver& resolver
                    , Queue& queue
                    , Message&& message)
    {
      queue.push(std::forward<Message>(message));
      resolver.resolve();
    }
  };
}

#endif
