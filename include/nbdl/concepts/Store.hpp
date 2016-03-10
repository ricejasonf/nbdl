//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPTS_STORE_HPP
#define NBDL_CONCEPTS_STORE_HPP

#include<nbdl/fwd/dispatch.hpp>

#include<boost/hana/is_default.hpp>
#include<boost/hana/tag_of.hpp>
#include<boost/hana/when.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  template<typename T>
  struct Store
  {
    using Tag = typename hana::tag_of<T>::type;
    static constexpr bool value = !hana::is_default<nbdl::dispatch_impl<Tag>>::value;
  };
} // nbdl

#endif
