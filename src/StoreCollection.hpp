//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_STORE_COLLECTION_HPP
#define NBDL_STORE_COLLECTION_HPP

#include<boost/hana.hpp>
#include "Store.hpp"

namespace nbdl {

template<typename StoreMap_, typename ListenerHandler_>
class StoreCollection
{
  using StoreMap = decltype(hana::to<hana::map_tag>(StoreMap_{}));
  StoreMap stores;

  template<typename T>
  auto& getStore(T)
  {
    return stores[hana::type_c<T>];
  }

  public:

  StoreCollection() :
    stores(hana::to<hana::map_tag>(StoreMap_{}))
  {}

  template<typename PathType, typename T>
  void forceAssign(const PathType& path, T&& value)
  {
    getStore(path).forceAssign(path, std::forward<T>(value));
  }

  template<typename PathType, typename T>
  void suggestAssign(const PathType& path, T&& value)
  {
    getStore(path).suggestAssign(path, std::forward<T>(value));
  }
  template<typename PathType, typename RequestFn, typename... Fns>
  auto get(RequestFn request, const PathType path, Fns... fns)
  {
    return getStore(path).get(request, path, fns...);
  }

  //emitter interface
  template<typename PathType>
  void addListener(const PathType& path, const ListenerHandler_& listener)
  {
    getStore(path).addListener(path, listener);
  }
  template<typename PathType>
  void removeListener(const PathType& path, const ListenerHandler_& listener)
  {
    getStore(path).removeListener(path, listener);
  }
};

}//nbdl

#endif
