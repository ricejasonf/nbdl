//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_PRODUCER_INIT_HPP
#define NBDL_PRODUCER_INIT_HPP

#include <nbdl/concept/Producer.hpp>
#include <nbdl/fwd/producer_init.hpp>

namespace nbdl {
  template <Producer Producer>
  constexpr void producer_init_fn::operator()(Producer& p) const {
    using Tag = hana::tag_of_t<Producer>;
    using Impl = producer_init_impl<Tag>;

    return Impl::apply(p);
  };

  template <typename Tag>
  struct producer_init_impl : hana::default_ {
    // do nothing by default
    template <typename Producer>
    static constexpr void apply(Producer&) { }
  };
}

#endif

