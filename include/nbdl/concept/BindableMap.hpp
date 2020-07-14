//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_BINDABLE_MAP_HPP
#define NBDL_CONCEPT_BINDABLE_MAP_HPP

#include <boost/hana/core/default.hpp>
#include <boost/hana/core/tag_of.hpp>

namespace nbdl {
  namespace hana = boost::hana;

  template <typename Tag>
  struct bind_map_impl
    : hana::default_
  {
    static constexpr auto apply(...) = delete;
  };

  template <typename T>
  concept BindableMap = 
    !hana::is_default<nbdl::bind_map_impl<hana::tag_of_t<T>>>::value;
}

#endif
