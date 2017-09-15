//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_STRING_HPP
#define NBDL_CONCEPT_STRING_HPP

#include <nbdl/fwd/concept/String.hpp>

#include <type_traits>

namespace nbdl
{
  namespace hana = boost::hana;

  namespace detail
  {
    template <typename S, typename Tag = hana::tag_of_t<S>>
    struct String_dispatch
      : std::integral_constant<bool, String<Tag>::value>
    { };

    template <typename S>
    struct String_dispatch<S, S>
      : std::false_type
    { };
  }

  template<typename S>
  struct String : detail::String_dispatch<S>
  { };
}

#endif
