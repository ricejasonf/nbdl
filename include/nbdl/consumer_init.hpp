//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONSUMER_INIT_HPP
#define NBDL_CONSUMER_INIT_HPP

#include <nbdl/concept/Consumer.hpp>
#include <nbdl/fwd/consumer_init.hpp>

namespace nbdl
{
  template <Consumer Consumer>
  constexpr void consumer_init_fn::operator()(Consumer& p) const {
    using Tag = hana::tag_of_t<Consumer>;
    using Impl = consumer_init_impl<Tag>;

    return Impl::apply(p);
  };

  template <typename Tag>
  struct consumer_init_impl : hana::default_ {
    // do nothing by default
    template <typename Consumer>
    static constexpr void apply(Consumer&) { }
  };
}

#endif

