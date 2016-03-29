//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_ENTITY_HPP
#define NBDL_CONCEPT_ENTITY_HPP

#include<nbdl/fwd/concept/Entity.hpp>

#include<nbdl/entity_members.hpp>

#include<boost/hana/core/default.hpp>
#include<type_traits>

namespace nbdl
{
  namespace hana = boost::hana;

  template<typename T>
  struct Entity
  {
    static constexpr bool value = !hana::is_default<nbdl::entity_members_impl<std::decay_t<T>>>::value;
  };
} // nbdl

#endif
