//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_CONTAINER_HPP
#define NBDL_CONCEPT_CONTAINER_HPP

#include<mpdef/list.hpp>
#include<nbdl/fwd/concept/Container.hpp>

#include <array> // for std::tuple_size
#include <boost/hana/integral_constant.hpp>
#include <boost/hana/type.hpp>
#include <iterator>
#include <utility>

namespace nbdl
{
  namespace hana = boost::hana;

  namespace detail
  {
    constexpr auto is_container = hana::is_valid([](auto&& x)
      -> mpdef::list<decltype(std::size(x)), decltype(std::begin(x)), decltype(std::end(x))>
    { return {}; });

    constexpr auto is_contiguous = hana::is_valid([](auto&& x)
      -> decltype(std::data(x))
    { return {}; });

    constexpr auto is_byte_container = hana::is_valid([](auto&& x)
      -> std::enable_if_t<(sizeof(typename std::decay_t<decltype(x)>::value_type) == 1)>
    { return {}; });

    constexpr auto is_size_fixed = hana::is_valid([](auto&& x)
      -> hana::size_t<std::tuple_size<std::decay_t<decltype(x)>>::value>
    { return {}; });
  }


  template <typename T>
  struct Container
    : decltype(detail::is_container(std::declval<T>()))
  { };

  template <>
  struct Container<void> : hana::false_
  { };
}

#endif
