//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_STORE_IMPL_HPP
#define NBDL_DEF_BUILDER_STORE_IMPL_HPP

#include "../builder.hpp"

namespace nbdl_def {
namespace builder {

constexpr auto store_impls = 
  hana::make_map(
    hana::make_pair(
      tag::HashMap,
      hana::type_c<nbdl::store::HashMap<Path_>>
    )
  );

template<typename DefPath>
class StoreImpl
{
  static constexpr auto def = hana::at_c<0>(DefPath{});

  public:

  using Tag = tag::AccessPoint_t;

  constexpr auto build()
  {
    constexpr auto setting = 
      hana::maybe(
        tag::HashMap, 
        //todo make function to get type from a map or use Type directive
        //to just return the type
        hana::partial(hana::at_key, store_impls) ^hana::on^ ,
        meta::findSetting(DefPath{}, tag::Store)
      );
  }
};

}//builder
}//nbdl_def
#endif
