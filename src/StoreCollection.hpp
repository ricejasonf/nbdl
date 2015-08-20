#ifndef NBDL_STORE_COLLECTION_HPP
#define NBDL_STORE_COLLECTION_HPP

#include<tuple>
#include "LambdaTraits.hpp"
#include "mpl/Unique.hpp"
#include "mpl/TupleGetByType.hpp"
#include "Store.hpp"

namespace nbdl {

namespace detail {

template<typename ApiDef, typename = CreateNumberSequence<ApiDef::size>>
struct StoreTupleFromApiApiDefinition;
template<typename ApiDef, typename Ns...>
struct StoreTupleFromApiApiDefinition<ApiDef, NumberSequence<Ns...>>
{
	using Type = typename mpl::Unique<std::tuple<Store<typename ApiDef::GetPath<Ns>::Type::Path::Entity> ...>>::Type;
};

}//detail

template<typename ApiDef>
class StoreCollection
{
	using Tuple	= typename EntityTupleFromApiDefinition<ApiDef>::Type;

	Tuple tuple;

	public:

	template<typename PathType>
	using VariantType = typename Store<typename PathType::Entity>::VariantType;

	template<typename PathType, typename T>
	void forceAssign(PathType path, T value)
	{
		using StoreType = Store<typename PathType::Entity>;
		StoreType& store = TupleGetByType<StoreType>::get(tuple);
		store.forceAssign(path, value);
	}

	template<typename PathType, typename T>
	void suggestAssign(PathType path, T value)
	{
		StoreType& store = TupleGetByType<StoreType>::get(tuple);
		store.suggestAssign(path, value);
	}
	template<typename PathType, typename RequestFn, typename Fn1, typename... Fns>
	typename LambdaTraits<Fn1>::ReturnType get(RequestFn request, const PathType path, Fn1 fn, Fns... fns)
	{
		StoreType& store = TupleGetByType<StoreType>::get(tuple);
		return store.get(path, value);
	}

	//todo purge function
};

}//nbdl

#endif
