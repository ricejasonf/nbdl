//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_STORE_COLLECTION_HPP
#define NBDL_STORE_COLLECTION_HPP

#include<tuple>
#include "LambdaTraits.hpp"
#include "mpl/NumberSequence.hpp"
#include "mpl/Unique.hpp"
#include "mpl/TupleGetByType.hpp"
#include "Store.hpp"

namespace nbdl {

namespace details {

template<typename ApiDef, typename = void>
struct StoreTupleFromApiDefinition
{
	using Type = typename StoreTupleFromApiDefinition<ApiDef, typename mpl::CreateNumberSequence<ApiDef::size>::Type>::Type;
};
template<typename ApiDef, int... sequence>
struct StoreTupleFromApiDefinition<ApiDef, mpl::NumberSequence<sequence...>>
{
	template<int n>
	using Store_ = Store<typename ApiDef::template GetPath<n>::Type::Path>;

	using Type = typename mpl::Unique<std::tuple<Store_<sequence>...>>::Type;
};

}//details

template<typename ApiDef>
class StoreCollection
{
	using Tuple	= typename details::StoreTupleFromApiDefinition<ApiDef>::Type;

	Tuple tuple;

	public:

	template<typename PathType>
	using VariantType = typename Store<PathType>::VariantType;

	template<typename PathType, typename T>
	void forceAssign(PathType path, T&& value)
	{
		using StoreType = Store<PathType>;
		StoreType& store = mpl::TupleGetByType<StoreType, Tuple>::get(tuple);
		store.forceAssign(path, std::forward<T>(value));
	}

	template<typename PathType, typename T>
	void suggestAssign(PathType path, T&& value)
	{
		using StoreType = Store<PathType>;
		StoreType& store = mpl::TupleGetByType<StoreType, Tuple>::get(tuple);
		store.suggestAssign(path, std::forward<T>(value));
	}
	template<typename PathType, typename RequestFn, typename Fn1, typename... Fns>
	typename LambdaTraits<Fn1>::ReturnType get(RequestFn request, const PathType path, Fn1 fn, Fns... fns)
	{
		using StoreType = Store<PathType>;
		StoreType& store = mpl::TupleGetByType<StoreType, Tuple>::get(tuple);
		return store.get(request, path, fn, fns...);
	}

	//todo purge function
};

}//nbdl

#endif
