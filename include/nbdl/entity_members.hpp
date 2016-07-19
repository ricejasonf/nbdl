//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_ENTITY_MEMBERS_HPP
#define NBDL_ENTITY_MEMBERS_HPP

#include <nbdl/fwd/entity_members.hpp>

#include <boost/hana/core/default.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  template <typename E>
  struct entity_members_impl
    : hana::default_
  { };

  template <typename Member>
  template <typename Entity>
  decltype(auto) get_member_fn<Member>::operator()(Entity&& e) const
  {
    return std::forward<Entity>(e).*Member::ptr;
  };
} // nbdl

#endif
