//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPTS_STORE_HPP
#define NBDL_CONCEPTS_STORE_HPP

#include<nbdl/fwd/make_store.hpp>
#include<nbdl/fwd/apply_action.hpp>
#include<nbdl/fwd/has.hpp>
#include<nbdl/fwd/get.hpp>

#include<boost/hana/core/default.hpp>
#include<boost/hana/core/tag_of.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  template<typename T>
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
