//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_STORE_MAP_DEFINITION_HPP
#define NBDL_DEF_STORE_MAP_DEFINITION_HPP

#include "def.hpp"

namespace nbdl_ddl {
namespace def {

template<typename ContextDef>
auto createStoreMapStorage(ContextDef);

//wrapper to instantiate a hana map for stores
template<typename ContextDef>
class StoreMap
{
  using Storage = decltype(createStoreMapStorage(ContextDef{}));

  Storage storage;

  public:

  StoreMap() :
    storage(createStoreMapStorage(ContextDef{}))
  {}

  template<typename KeyType>
  auto& get(KeyType)
  {
    return storage[hana::type_c<KeyType>];
  }

};

}//def
}//nbdl_ddl

#endif
