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

template<typename Context, typename = void>
struct StoreTupleFromContext
{
	using ApiDef = typename Context::ApiDef;
	using Type = typename StoreTupleFromContext<Context, typename mpl::CreateNumberSequence<ApiDef::size>::Type>::Type;
};
template<typename Context, int... sequence>
struct StoreTupleFromContext<Context, mpl::NumberSequence<sequence...>>
{
	using ApiDef = typename Context::ApiDef;
	template<int n>
	using Store_ = Store<Context, typename ApiDef::template GetPath<n>::Type::Path>;

	using Type = typename mpl::Unique<std::tuple<Store_<sequence>...>>::Type;
};

}//details

template<typename Context>
class StoreCollection
{
	using ListenerHandler = typename Context::ListenerHandler;
	using ApiDef = typename Context::ApiDef;
	using Tuple	= typename details::StoreTupleFromContext<Context>::Type;

	Tuple tuple;

	public:

	template<typename PathType>
	using VariantType = typename Store<Context, PathType>::VariantType;

	template<typename PathType, typename T>
	void forceAssign(PathType path, T&& value)
	{
		using StoreType = Store<Context, PathType>;
		StoreType& store = mpl::TupleGetByType<StoreType, Tuple>::get(tuple);
		store.forceAssign(path, std::forward<T>(value));
	}

	template<typename PathType, typename T>
	void suggestAssign(PathType path, T&& value)
	{
		using StoreType = Store<Context, PathType>;
		StoreType& store = mpl::TupleGetByType<StoreType, Tuple>::get(tuple);
		store.suggestAssign(path, std::forward<T>(value));
	}
	template<typename PathType, typename RequestFn, typename Fn1, typename... Fns>
	typename LambdaTraits<Fn1>::ReturnType get(RequestFn request, const PathType path, Fn1 fn, Fns... fns)
	{
		using StoreType = Store<Context, PathType>;
		StoreType& store = mpl::TupleGetByType<StoreType, Tuple>::get(tuple);
		return store.get(request, path, fn, fns...);
	}

	//emitter interface
	template<typename PathType>
	void removeListener(const PathType& path, const ListenerHandler& listener)
	{
		using StoreType = Store<Context, PathType>;
		StoreType& store = mpl::TupleGetByType<StoreType, Tuple>::get(tuple);
		store.removeListener(path, listener);
	}
};

}//nbdl

#endif
