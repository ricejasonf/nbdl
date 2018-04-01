//
// Copyright Jason Rice 2018
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_ACTOR_TYPE_HPP
#define NBDL_FWD_ACTOR_TYPE_HPP

#include <boost/hana/core/tag_of.hpp>
#include <boost/hana/pair.hpp>
#include <boost/hana/type.hpp>

namespace nbdl
{
  template <typename Tag, typename Context>
  struct actor_type
  {
    using type = typename Tag::template actor_impl<Context>;
  };

  template <typename T, typename Context>
  using actor_type_t = typename actor_type<boost::hana::tag_of_t<T>, Context>::type;

  //
  // actor_initializer  - actors must be constructible with an rvalue of this where
  //                      Value is the value provided in the context constructor
  //

  template <typename Context, typename Value = boost::hana::type<void>>
  struct actor_initializer
  {
    Context context;
    Value value;
  };
}

#endif
