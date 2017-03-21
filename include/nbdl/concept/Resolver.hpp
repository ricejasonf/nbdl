//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_RESOLVER_HPP
#define NBDL_CONCEPT_RESOLVER_HPP

#include <nbdl/fwd/concept/Resolver.hpp>

#include <boost/hana/core/tag_of.hpp>
#include <boost/hana/integral_constant.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  // Just opt-in like hana::Sequence
  // TODO require members resolve/reject
  namespace detail
  {
    template <typename T, typename Tag = typename hana::tag_of<T>::type>
    struct Resolver_dispatch
      : hana::bool_<nbdl::Resolver<Tag>::value>
    { };

    template <typename T>
    struct Resolver_dispatch<T, T>
      : hana::false_
    { };
  }

  template <typename T>
  struct Resolver
    : detail::Resolver_dispatch<T>
  { };
}

#endif
