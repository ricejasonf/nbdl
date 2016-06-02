//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_NULL_STORE_HPP
#define NBDL_NULL_STORE_HPP

#include<nbdl/fwd/null_store.hpp>

#include<nbdl/make_store.hpp>
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
  struct make_store_impl<null_store>
  {
    template <typename PathType>
    static constexpr auto apply(PathType)
      -> nbdl::null_store
    { return {}; }
  };

  template <>
  struct apply_action_impl<null_store>
  {
    template <typename Store, typename Message>
    static constexpr bool apply(Store&&, Message&&)
    {
      // changes nothing
      return false;
    }
  };

  template <>
  struct get_impl<null_store>
  {
    template <typename Store, typename Path>
    static constexpr auto apply(Store&&, Path&&)
      -> nbdl::uninitialized
    { return {}; }
  };
} // nbdl

#endif
