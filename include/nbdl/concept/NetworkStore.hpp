//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_NEWTORK_STORE_HPP
#define NBDL_CONCEPT_NEWTORK_STORE_HPP

#include<nbdl/fwd/concept/NetworkStore.hpp>
#include<nbdl/fwd/apply_message.hpp>

#include<nbdl/apply_foreign_message.hpp>
#include<nbdl/apply_message.hpp>

#include<boost/hana/core/default.hpp>
#include<boost/hana/core/tag_of.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  template <typename T>
  struct NetworkStore
  {
    using Tag = typename hana::tag_of<T>::type;
    static constexpr bool value = (
       !hana::is_default<nbdl::apply_message_impl         <Tag>>::value
    || !hana::is_default<nbdl::apply_foreign_message_impl <Tag>>::value
    );
  };
}

#endif
