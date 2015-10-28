//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_STORE_COLLECTION_HPP
#define NBDL_STORE_COLLECTION_HPP

#include<boost/hana.hpp>
#include "LambdaTraits.hpp"
#include "Store.hpp"

namespace nbdl {

namespace details {

}//details

template<typename Context>
class StoreCollection
{
  using ListenerHandler = typename Context::ListenerHandler;
  using ApiDef = typename Context::ApiDef;
  using Stores = decltype(details::storeMap(hana::type_t<Context>))::type;

  Stores stores;

  template<typename PathType>
  auto& getStore(PathType)
  {
    //hoping this returns a reference???
    return stores[hana::type_t<PathType>];
  } 

  public:

  template<typename PathType>
  using VariantType = typename Store<Context, PathType>::VariantType;

  template<typename PathType, typename T>
  void forceAssign(const PathType& path, T&& value)
  {
    auto& store = getStore(path);
    store.forceAssign(path, std::forward<T>(value));
  }

  template<typename PathType, typename T>
  void suggestAssign(const PathType& path, T&& value)
  {
    auto& store = getStore(path);
    store.suggestAssign(path, std::forward<T>(value));
  }
  template<typename PathType, typename RequestFn, typename Fn1, typename... Fns>
  typename LambdaTraits<Fn1>::ReturnType
  get(RequestFn request, const PathType path, Fn1 fn, Fns... fns) const
  {
    const auto& store = getStore(path);
    return store.get(request, path, fn, fns...);
  }

  //emitter interface
  template<typename PathType>
  void addListener(const PathType& path, const ListenerHandler& listener)
  {
    auto& store = getStore(path);
    store.addListener(path, listener);
  }
  template<typename PathType>
  void removeListener(const PathType& path, const ListenerHandler& listener)
  {
    auto& store = getStore(path);
    store.removeListener(path, listener);
  }
};

}//nbdl

#endif
