//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_STORE_HPP
#define NBDL_STORE_HPP

#include "LambdaTraits.hpp"
#include "store/HashMap.hpp"

namespace nbdl {

template<typename PathType>
struct StoreImpl
{
	using Type = store::HashMap<PathType>;
};

template<typename PathType>
class Store
{
	using Impl = typename StoreImpl<PathType>::Type;

	Impl impl;

	public:

	using VariantType = typename Impl::VariantType;

	template<typename T>
	void forceAssign(PathType path, T&& value)
	{
		impl.assign(path, std::forward<T>(value));
	}

	template<typename T>
	void suggestAssign(PathType path, T&& value)
	{
		impl.get(path).match(
			[&](Unresolved) {
				impl.assign(path, std::forward<T>(value));
			});
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

	//todo purge function
};

}//nbdl

#endif
