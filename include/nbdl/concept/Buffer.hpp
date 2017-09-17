//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_BUFFER_HPP
#define NBDL_CONCEPT_BUFFER_HPP

#include <nbdl/concept/Container.hpp>
#include <nbdl/fwd/concept/Buffer.hpp>

#include <iterator>
#include <type_traits>

namespace nbdl
{
  namespace hana = boost::hana;

  template<typename T>
  struct Buffer
  {
    using Tag = typename std::decay<T>::type;
    static constexpr bool value = nbdl::Container<Tag>::value
                               && decltype(detail::is_contiguous(std::declval<Tag>()))::value
                               && decltype(detail::is_byte_container(std::declval<Tag>()))::value
                               && decltype(detail::is_size_fixed(std::declval<Tag>()))::value
                               ;
  };
}

#endif
