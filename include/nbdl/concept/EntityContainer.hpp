//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_ENTITY_CONTAINER_HPP
#define NBDL_CONCEPT_ENTITY_CONTAINER_HPP

#include <nbdl/fwd/concept/EntityContainer.hpp>

#include <nbdl/concept/Container.hpp>
#include <nbdl/concept/Entity.hpp>

#include <boost/mp11/utility.hpp>

namespace nbdl
{
  namespace detail
  {
    using namespace boost::mp11;
    template <typename T>
    using value_type = typename T::value_type;

    template <typename T>
    using value_type_or_void = mp_eval_if<mp_not<mp_valid<value_type, T>>, void, value_type, T>;
  }

  template<typename T>
  struct EntityContainer
  {
    static constexpr bool value =
          nbdl::Container<T>::value
      and nbdl::Entity<detail::value_type_or_void<T>>::value
    ;
  };
} // nbdl

#endif
