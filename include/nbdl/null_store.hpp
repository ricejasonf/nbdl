//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_NULL_STORE_HPP
#define NBDL_NULL_STORE_HPP

#include<nbdl/fwd/make_store.hpp>
#include<nbdl/fwd/null_store.hpp>

#include<nbdl/apply_action.hpp>
#include<nbdl/get.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  struct null_store
  {
    // stores nothing
  };

  template <>
  struct apply_action_impl<null_store>
  {
    template <typename Store, typename Message>
    static constexpr auto apply(Store&&, Message&&)
    {
      // changes nothing
      return hana::false_c;
    }
  };

  template <>
  struct get_impl<null_store>
  {
    template <typename Store, typename Key>
    static constexpr auto apply(Store&&, Key&&)
      -> nbdl::uninitialized
    { return {}; }
  };
} // nbdl

#endif
