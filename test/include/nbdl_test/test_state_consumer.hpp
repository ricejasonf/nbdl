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
  struct state_consumer { };

  template <typename PushApi>
  struct state_consumer_impl
  {
    using hana_tag = state_consumer;
    PushApi push_api;
  };
}

namespace nbdl
{
  template <>
  struct make_state_consumer_impl<nbdl_test::state_consumer>
  {
    template <typename PushApi>
    static auto apply(PushApi&& p)
    {
      return nbdl_test::state_consumer_impl<std::decay_t<PushApi>>{std::forward<PushApi>(p)};
    }
  };

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
