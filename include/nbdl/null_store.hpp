//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_NULL_STORE_HPP
#define NBDL_NULL_STORE_HPP

#include<nbdl/fwd/null_store.hpp>
#include<nbdl/fwd/make_store.hpp>
#include<nbdl/fwd/apply_action.hpp>
#include<nbdl/fwd/get.hpp>
#include<nbdl/fwd/has.hpp>

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
    template <typename Path>
    static constexpr auto apply(Path&&)
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

  // really I don't see `get` or `has` ever being called
  template <>
  struct get_impl<null_store>
  {
    template <typename Store, typename Path>
    static constexpr auto apply(Store&&, Path&&)
      -> nbdl::not_found
    { return {}; }
  };

  template <>
  struct has_impl<null_store>
  {
    template <typename Store, typename Path>
    static constexpr bool apply(Store&&, Path&&)
    {
      // has nothing
      return true;
    }
  };
} // nbdl

#endif
