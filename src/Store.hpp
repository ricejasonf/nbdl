#ifndef NBDL_STORE_HPP
#define NBDL_STORE_HPP

#include "LambdaTraits.hpp"
#include "store/HashMap.hpp"

namespace nbdl {

template<typename Entity>
struct StorePath {};

template<typename Entity>
struct StoreImpl
{
	using Type = store::HashMap<typename StorePath<Entity>::Type, Entity>;
};

template<typename Entity>
class Store
{
	using PathType = typename StorePath<Entity>::Type;
	using Impl = typename StoreImpl<Entity>::Type;

	Impl impl;

	public:

	template<typename T>
	void forceAssign(PathType path, T value)
	{
		impl.assign(path, value);
	}

	template<typename T>
	void suggestAssign(PathType path, T value)
	{
		if (!impl.hasEntry(path))
			impl.assign(path, value);
	}

	template<typename RequestFn, typename Fn1, typename... Fns>
	typename LambdaTraits<Fn1>::ReturnType get(RequestFn request, const PathType path, Fn1 fn, Fns... fns)
	{
		if (!impl.hasEntry(path))
		{
			impl.assign(path, Unresolved{});
			request(path);
		}
		return impl.get(path).match(fn, fns...);
	}
};

}//nbdl

#endif
