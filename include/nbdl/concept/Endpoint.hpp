//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_ENDPOINT_HPP
#define NBDL_CONCEPT_ENDPOINT_HPP

#include <nbdl/concept/HasImpl.hpp>

#include <boost/hana/core/tag_of.hpp>

namespace nbdl {
  template <typename T>
  struct endpoint_send_message_impl;

  template <typename T>
  struct endpoint_send_close_impl;

  // opt-in for use of default implementations 
  // via specialization of `is_endpoint`
  template <typename T>
  inline constexpr bool is_endpoint =
                     HasImpl<T, nbdl::endpoint_send_message_impl> &&
                     HasImpl<T, nbdl::endpoint_send_close_impl>;

  template <typename T>
  concept Endpoint = is_endpoint<hana::tag_of_t<T>>;

  template <typename T>
  concept EndpointPtr = Endpoint<typename T::element_type>;
}

#endif
