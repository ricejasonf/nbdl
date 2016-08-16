//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MAKE_DEF_HPP
#define NBDL_MAKE_DEF_HPP

#include <nbdl/fwd/make_def.hpp>

namespace nbdl
{
  template <typename T>
  constexpr auto make_def_fn::operator()(T) const
  {
    using Tag  = typename T::type;
    using Impl = make_def_impl<Tag>;

    return Impl::apply();
  };

  template <typename Tag, bool condition>
  struct make_def_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    static constexpr auto apply()
    {
      return Tag::make_def();
    }
  };
}

#endif
