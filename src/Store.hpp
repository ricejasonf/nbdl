//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_STORE_HPP
#define NBDL_STORE_HPP

#include "store/HashMap.hpp"
#include "store_emitter/HashMap.hpp"

namespace nbdl {

//todo maybe create a way to set default store containers on a per Context basis
template<typename Context, typename PathType>
struct StoreImpl
{
	using Type = store::HashMap<PathType>;
};

template<typename Context, typename PathType>
struct StoreEmitterImpl
{
	using Type = store_emitter::HashMap<PathType, typename Context::ListenerHandler>;
	using ListenerHandler = typename Context::ListenerHandler;
};

template<
  typename Impl,
  typename EmitterImpl,
  typename ListenerHandler,
  typename PathType >
class Store
{
	using Entity = typename PathType::Entity;

	Impl impl;
	EmitterImpl emitter;

	template<typename T>
	void emitChange(const PathType& path, const T& value)
	{
		emitter.emit(path, [&](ListenerHandler& listener) {
			listener.notify_(value);
		});
	}

	public:

	using VariantType = typename Impl::VariantType;

	/*
	void action(Update action)
	{
		VariantType& value = impl.get(action.path);
		//this doesn't use assign
		value.match(
			[&](Entity entity) {
				action.patch(entity);	
				value = entity;
			});
	}
	*/

	template<typename T>
	void forceAssign(const PathType& path, T&& value)
	{
		emitChange(path, impl.assign(path, std::forward<T>(value)));
	}

	template<typename T>
	void suggestAssign(const PathType& path, T&& val)
	{
		VariantType& value = impl.get(path);
		bool unresolved = value.match(
			[](Unresolved) {
				return true;
			},
			[](auto) {
				return false;
			});
		if (unresolved)
		{
			value = std::forward<T>(val);
			emitChange(path, value);
		}
	}

	template<typename RequestFn, typename... Fns>
	auto get(RequestFn request, const PathType& path, Fns... fns)
	{
		if (!impl.hasEntry(path))
		{
			impl.assign(path, Unresolved{});
			request(path);
		}
		return impl.get(path).match(fns...);
	}

	//emitter interface
	void addListener(const PathType& path, const ListenerHandler& listener)
	{
		emitter.addListener(path, listener);
	}
	void removeListener(const PathType& path, const ListenerHandler& listener)
	{
		emitter.removeListener(path, listener);
	}
};

}//nbdl

#endif
