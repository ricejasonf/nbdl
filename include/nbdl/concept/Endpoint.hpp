//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_ENDPOINT_HPP
#define NBDL_CONCEPT_ENDPOINT_HPP

#include <nbdl/fwd/concept/Endpoint.hpp>

#include <nbdl/endpoint_send_close.hpp>
#include <nbdl/endpoint_send_message.hpp>

#include <boost/hana/core/default.hpp>
#include <boost/hana/core/tag_of.hpp>
#include <type_traits>

namespace nbdl
{
  namespace hana = boost::hana;

  // Allows opt-in just like hana::Sequence
  namespace detail
  {
    template <typename E, typename Tag = typename hana::tag_of<E>::type>
    struct endpoint_dispatch
      : std::integral_constant<bool, nbdl::Endpoint<Tag>::value>
    { };

    template <typename E>
    struct endpoint_dispatch<E, E>
      : std::false_type
    { };
  }


  template<typename T>
  struct Endpoint
    : std::integral_constant<bool,
           detail::endpoint_dispatch<T>::value
        || (!hana::is_default<nbdl::endpoint_send_message_impl<hana::tag_of_t<T>>>::value 
        &&  !hana::is_default<nbdl::endpoint_send_close_impl  <hana::tag_of_t<T>>>::value)
      >
  { };
}

#endif
