//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_CONSUMER_HPP
#define NBDL_CONCEPT_CONSUMER_HPP

#include <nbdl/fwd/concept/Consumer.hpp>
#include <nbdl/send_downstream_message.hpp>

#include <boost/hana/core/default.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  template<typename T>
  struct Consumer
  {
    using Tag = typename hana::tag_of<T>::type;
    static constexpr bool value =
          !hana::is_default<nbdl::send_downstream_message_impl  <Tag>>::value
      ;
  };
}

#endif
