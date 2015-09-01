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
#include "store_emitter/HashMap.hpp"

namespace nbdl {

//todo maybe create a way to set default store containers on a per Context basis
template<typename PathType>
struct StoreImpl
{
	using Type = store::HashMap<PathType>;
};

template<typename PathType>
struct StoreEmitterImpl
{
	using Type = store_emitter::HashMap<PathType>;
};

template<typename PathType>
class Store
{
	using Impl = typename StoreImpl<PathType>::Type;
	using EmitterImpl = typename StoreEmitterImpl<PathType>::Type;
	//todo listener types will determined by Client type... somehow
	//using Listener = uhhh

	Impl impl;
	EmitterImpl emitter;

	void emitChange(const PathType& path)
	{
		emitter.emit(path, [](const PathType& path, Listener listener) {
			listener.onChange(path);
		});
	}

	public:

	using VariantType = typename Impl::VariantType;

	template<typename T>
	void forceAssign(PathType path, T&& value)
	{
		impl.assign(path, std::forward<T>(value));
		emitChange(path);
	}

	template<typename T>
	void suggestAssign(PathType path, T&& value)
	{
		impl.get(path).match(
			[&](Unresolved) {
				impl.assign(path, std::forward<T>(value));
				emitChange(path);
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
