//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_CONSUMER_HPP
#define NBDL_CONCEPT_CONSUMER_HPP

#include <nbdl/concept/HasImpl.hpp>

namespace nbdl {
  template <typename Tag>
  struct send_downstream_message_impl : default_impl {
    static constexpr auto apply(...) = delete;
  };

  template <typename T>
  concept Consumer = HasImpl<T, nbdl::send_downstream_message_impl>;
}

#endif
