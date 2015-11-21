//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_PATH_HPP
#define NBDL_PATH_HPP

#include<utility>
#include<type_traits>
#include<tuple>
#include<boost/hana.hpp>
//#include<boost/functional/hash.hpp>

#include "details/Hash.hpp"
#include "mpl/Reverse.hpp"
#include "EntityTraits.hpp"
#include "EntityKey.hpp"

namespace nbdl {

namespace hana = boost::hana;

template<typename Spec>
class Path
{
  static constexpr auto spec() { return Spec{}; }

  static auto parents()
  {
    return hana::unpack(spec(), 
      [](auto... j) {
        return hana::make_tuple(hana::first(j)...);
      });
  }

  static auto entityTypes()
  {
    constexpr auto spec_ = spec();
    return hana::unpack(
      hana::range_c<int, 0, hana::length(spec_)>,
      [&](auto... i) {
        return hana::make_map(hana::make_pair(
          hana::at(parents(), i), i)...);
      });
  }

  template<typename T>
  static constexpr auto entityTypeIndex(T type)
  {
    auto types = entityTypes();
    return types[type];
  }

  static auto storageType()
  {
    return hana::unpack(spec(),
      [](auto... t) {
        return hana::type_c<decltype(hana::make_tuple(
            std::declval<typename decltype(+hana::second(t))::type>()...
          ))>;
      });
  }

  using Key = typename decltype(+hana::second(hana::back(spec())))::type;
  using ParentPath = Path<decltype(hana::drop_back(spec(), hana::size_c<1>))>;
  using Storage = typename decltype(storageType())::type;

  public:

  Storage storage;

  using Entity = typename decltype(+hana::first(hana::back(spec())))::type;

  static_assert(IsEntity<Entity>::value, "");

  template<typename... Args>
  Path(Args... args) :
    storage(hana::make_tuple(args...))
  {}

  template<typename E = Entity>
  Key getKey() const
  {
    return hana::at(storage, entityTypeIndex(hana::type_c<E>));
  }

  ParentPath parent() const
  {
    return hana::unpack(hana::drop_back(storage, hana::size_c<1>),
      [](auto... n) {
        return ParentPath(n...);
      });
  }

  struct HashFn
  {
    std::size_t operator()(const Path& path) const
    {
      std::size_t seed;
      hana::for_each(path.storage,
        [&](const auto& x) {
          nbdl::details::HashCombine(seed, x); 
        });
      return seed;
    }
  };

  struct PredFn
  {
    bool operator()(const Path& t1, const Path& t2) const
    {
      return t1.storage == t2.storage;
    }
  };

};

template<typename... Pairs>
constexpr auto makePathTypeFromPairs(Pairs... pairs)
{
  using Spec = decltype(hana::make_tuple(pairs...));
  return hana::type_c<Path<Spec>>;
}

template<typename Key, typename... EntityType>
constexpr auto path_type = makePathTypeFromPairs(
    hana::make_pair(hana::type_c<EntityType>, hana::type_c<Key>)...
  );

}//nbdl

#endif
