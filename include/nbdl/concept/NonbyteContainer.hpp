//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_NONBYTE_CONTAINER_HPP
#define NBDL_CONCEPT_NONBYTE_CONTAINER_HPP

#include <nbdl/concept/Buffer.hpp>
#include <nbdl/concept/Container.hpp>
#include <nbdl/concept/DynamicBuffer.hpp>
#include <nbdl/concept/String.hpp>
#include <nbdl/fwd/concept/NonbyteContainer.hpp>

#include <type_traits>

namespace nbdl
{
  namespace hana = boost::hana;

  template<typename T>
  struct NonbyteContainer
  {
    using Tag = typename std::decay<T>::type;
    static constexpr bool value = nbdl::Container<Tag>::value
                          and not nbdl::String<Tag>::value
                          and not nbdl::Buffer<Tag>::value
                          and not nbdl::DynamicBuffer<Tag>::value
                               ;
  };
}

#endif
