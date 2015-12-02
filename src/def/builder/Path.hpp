//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_PATH_HPP
#define NBDL_DEF_BUILDER_PATH_HPP

#include<type_traits>
#include "../builder.hpp"
#include "../directives.hpp"
#include "../../Path.hpp"

namespace nbdl_def {
namespace builder {

template<typename AccessPointDef>
constexpr auto path(AccessPointDef access_point)
{
  auto path_def = *meta::findByTag(*meta::findByTag(access_point, tag::Path), tag::Type);
  //todo actually use a path definition instead of embedding the type
  return hana::at(path_def, hana::int_c<0>);
}

}//builder
}//nbdl_def
#endif
