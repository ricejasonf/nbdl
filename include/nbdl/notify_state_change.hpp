//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_NOTIFY_STATE_CHANGE_HPP
#define NBDL_NOTIFY_STATE_CHANGE_HPP

#include<nbdl/fwd/notify_state_change.hpp>

#include<nbdl/concept/StateConsumer.hpp>

#include<utility>

namespace nbdl
{
  template<typename StateConsumer, typename Path>
  constexpr auto notify_state_change_fn::operator()(StateConsumer&& s, Path&& p) const
  {
    using Tag = hana::tag_of_t<StateConsumer>;
    using Impl = notify_state_change_impl<Tag>;

    static_assert(nbdl::StateConsumer<StateConsumer>::value,
      "nbdl::notify_state_change(consumer, path) requires 'consumer' to be a StateConsumer");

    return Impl::apply(std::forward<StateConsumer>(s), std::forward<Path>(p));
  };

  template<typename Tag, bool condition>
  struct notify_state_change_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    static constexpr auto apply(...) = delete;
  };
} // nbdl

#endif
