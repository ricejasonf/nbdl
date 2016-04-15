//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_STORE_HPP
#define NBDL_CONCEPT_STORE_HPP

#include<nbdl/fwd/concept/Store.hpp>

#include<nbdl/make_store.hpp>
#include<nbdl/apply_action.hpp>
#include<nbdl/has.hpp>
#include<nbdl/get.hpp>

#include<boost/hana/core/default.hpp>
#include<boost/hana/core/tag_of.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  template <typename T>
  struct Store
  {
    using Tag = typename hana::tag_of<T>::type;
    static constexpr bool value =
          !hana::is_default<nbdl::make_store_impl     <Tag>>::value
      &&  !hana::is_default<nbdl::apply_action_impl   <Tag>>::value
      &&  !hana::is_default<nbdl::has_impl            <Tag>>::value
      &&  !hana::is_default<nbdl::get_impl            <Tag>>::value
      ;
  };
} // nbdl

#endif
