//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NBDL_TEST_ASSETS_TEST_STATE_CONSUMER_HPP
#define NBDL_TEST_ASSETS_TEST_STATE_CONSUMER_HPP

#include <nbdl/concept/StateConsumer.hpp>

#include <boost/hana.hpp>
#include <utility>

namespace nbdl_test
{
  struct state_consumer;

  template <typename Context>
  struct state_consumer_impl
  {
    using hana_tag = state_consumer;
    Context context;

    template <typename T>
    state_consumer_impl(nbdl::actor_initializer<Context, T>&& a)
     : context(a.context)
    { } 
  };

  struct state_consumer
  {
    template <typename Context>
    using actor_impl = state_consumer_impl<Context>;
  };

}

namespace nbdl
{
  template <>
  struct notify_state_change_impl<nbdl_test::state_consumer>
  {
    template <typename StateConsumer, typename Path>
    static void apply(StateConsumer&&, Path&&)
    {
      // ???
    }
  };
}

#endif
