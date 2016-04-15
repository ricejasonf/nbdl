//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_STORE_HPP
#define NBDL_DEF_BUILDER_STORE_HPP

#include <def/builder/AccessPointMeta.hpp>
//#include<Store.hpp>
#include <nbdl/make_store.hpp>
#include <nbdl/null_store.hpp>

namespace nbdl_def {
namespace builder {

struct store_fn
{
  template<typename PathType, typename AccessPoint>
  constexpr auto operator()(PathType path_type, AccessPoint) const
  {
    return hana::type_c<decltype(nbdl::make_store<nbdl::null_store>(path_type))>;
  }
};
constexpr store_fn store{};

}//builder
}//nbdl_def
#endif
