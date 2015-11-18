//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_PATH_HPP
#define NBDL_PATH_HPP

#include<type_traits>
#include<tuple>
#include<boost/functional/hash.hpp>

#include "details/Hash.hpp"
#include "mpl/Reverse.hpp"
#include "EntityTraits.hpp"
#include "EntityKey.hpp"

namespace nbdl {

namespace path_details {

struct Bottom
{
  static constexpr auto entityTypes()
  {
    return hana::make_map();
  }
  using Storage = decltype(hana::make_tuple());
  Bottom() = delete;
  Bottom(const Bottom&) = delete;
};

}//path_details

template<typename Entity_, typename Key, typename ParentPath = path_details::Bottom>
class Path
{
  //uhh change this back
  static auto entityTypes()
  {
    return hana::unpack(
      hana::range_c<int, 0, sizeof...(ParentEntities) - 1>,
      [](auto... i) {
        constexpr auto parents = hana::tuple_t<ParentEntities...>;
        return hana::make_map(hana::make_pair(
          hana::at(parents, i), i)...);
      });
  }

  template<typename T>
  static constexpr auto entityTypeIndex(T type)
  {
    auto types = entityTypes();
    return types[type];
  }

	using Storage = decltype(hana::append(
    typename std::declval<typename ParentPath::Storage>(), std::declval<Key>()));

	Storage storage;

	public:

	using Entity = Entity_;

	Path(Key k, ParentPath p) : 
    storage(hana::append(p.storage, hana::type_c<Entity_>, k))
	{}

	template<typename... Args>
	Path(Args... args) :
    storage(hana::make_tuple(args...))
	{}

	template<typename E = Entity_>
	KeyType getKey() const
	{
    static constexpr entity_type_index = hana::at_key(entityTypes(), hana::type_c<E>);
		return hana::at(storage, entityTypeIndex());
	}

	ParentPath parent() const
	{
    return hana::drop_back(storage, hana::size_c<1>);
	}

  struct HashFn
  {
    auto operator()(std::size_t& seed, const Storage& storage)
    {
      for_each(storage, [&](const auto& x) {
          nbdl::details::HashCombine(seed, x); 
        });
    }
  };

  struct PredFn
  {
    bool operator() (const PathType& t1, const PathType& t2) const
    {
      return t1.storage == t2.storage;
    }
  };

};

//todo makePath function
template<typename... Args>
struct CreatePath
{
	using ReversedTuple = typename mpl::Reverse<std::tuple<Args...>>::Type;
	using Type = typename CreatePathFromReversedTuple<ReversedTuple>::Type;	
};

template<typename... Pairs>
constexpr auto makePathType(Pairs... pairs)
{
  using Spec = decltype(hana::make_tuple(pairs...));
  return hana::type_c<Path<Spec>>;
}

//This assumes all entities use int for their primary keys.
template<typename... EntityTypes>
constexpr auto makePathTypeFromEntities(EntityTypes... entity_types)
{
  return makePathType(entity_types, hana::type_c<int>)...;
}

}//nbdl

#endif
