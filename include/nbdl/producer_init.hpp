//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_PRODUCER_INIT_HPP
#define NBDL_PRODUCER_INIT_HPP

#include <nbdl/fwd/producer_init.hpp>

#include <nbdl/concept/Producer.hpp>

namespace nbdl
{
  template <typename Producer>
  constexpr void producer_init_fn::operator()(Producer& p) const
  {
    using Tag = hana::tag_of_t<Producer>;
    using Impl = producer_init_impl<Tag>;

    static_assert(
      nbdl::Producer<Tag>::value
    , "nbdl::producer_init<T>(p) require `p` to be a nbdl::Producer."
    );

    return Impl::apply(p);
  };

  template <typename Tag, bool condition>
  struct producer_init_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    // do nothing by default
    template <typename Producer>
    static constexpr void apply(Producer&) { }
  };
}

#endif

