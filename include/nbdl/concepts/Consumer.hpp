//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPTS_CONSUMER_HPP
#define NBDL_CONCEPTS_CONSUMER_HPP

#include<nbdl/fwd/make_consumer.hpp>
#include<nbdl/fwd/send_downstream_message.hpp>

#include<boost/hana/core/default.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  template<typename T>
  struct Consumer
  {
    static constexpr bool value =
          !hana::is_default<nbdl::send_downstream_message_impl  <T>>::value
      &&  !hana::is_default<nbdl::make_consumer_impl            <T>>::value
      ;
  };
} // nbdl

#endif
