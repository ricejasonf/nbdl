//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_BINDABLE_MAP_HPP
#define NBDL_CONCEPT_BINDABLE_MAP_HPP

#include<nbdl/fwd/concept/BindableMap.hpp>

#include<nbdl/bind_map.hpp>

#include<boost/hana/core/default.hpp>
#include<boost/hana/core/tag_of.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  template<typename T>
  struct BindableMap
  {
    using Tag = typename hana::tag_of<T>::type;
    static constexpr bool value = !hana::is_default<nbdl::bind_map_impl<Tag>>::value;
  };
} // nbdl

#endif
