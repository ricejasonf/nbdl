//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONTEXT_HPP
#define NBDL_CONTEXT_HPP

#include<memory>
#include "LambdaTraits.hpp"
#include "StoreCollection.hpp"

namespace nbdl {

namespace details {

template<typename Traits>
class Context : public std::enable_shared_from_this<Context<Traits>>
{
	public:

	using SharedPtr = std::shared_ptr<Context>;
	using WeakPtr = std::weak_ptr<Context>;
	using Client = typename Traits::Client;
	using ListenerHandler = typename Traits::ListenerHandler;
	template<typename PathType>
	using Listener = details::Listener<Context, PathType>;
	using ApiDef = typename Traits::ApiDef;

	private:

	Client client;
	StoreCollection<Context> store;

	public:

	//todo client probably wont be copyable
	Context(Client c) : client(c) {}

	template<typename PathType, typename... Args>
	Listener<PathType> makeListener(PathType path, Args... args)
	{
		auto handler = ListenerHandler(args...);
		auto wrapper = Listener<PathType>(path, this->shared_from_this(), ListenerHandler(args...));
		store.addListener(path, handler);
		return wrapper;
	}
	template<typename PathType>
	void addListener(const PathType& path, const ListenerHandler& listener)
	{
		store.addListener(path, listener);
	}
	template<typename PathType>
	void removeListener(const PathType& path, const ListenerHandler& listener)
	{
		store.removeListener(path, listener);
	}

	template<typename Path, typename MatchFn1, typename... MatchFns>
	typename LambdaTraits<MatchFn1>::ReturnType read(Path path, MatchFn1 fn1, MatchFns... fns)
	{
		using VariantType = typename StoreCollection<Context>::template VariantType<Path>;

		return store.get(
			//called if store needs to request value
			[&](const Path& path) {
				//request from client
				auto self = this->shared_from_this();
				client.read(path, [path, self](VariantType&& value) {
					self->store.suggestAssign(path, std::forward<VariantType>(value));
				});
			},
			path, fn1, fns...);
	}
};

template<
	typename ClientType,
	typename ListenerHandlerType,
	typename ApiDefType>
struct ContextTraits
{
	using Client = ClientType;
	using ListenerHandler = ListenerHandlerType;
	using ApiDef = ApiDefType;
};


}//details

template<
	typename ClientType,
	typename ListenerHandlerType,
	typename ApiDefType>
struct Context
{
	using Type = details::Context<details::ContextTraits<ClientType, ListenerHandlerType, ApiDefType>>;
	using SharedPtr = typename Type::SharedPtr;

	template<typename... Args>
	static SharedPtr create(Args... args)
	{
		return std::make_shared<Type>(args...);
	}
};
//using Context = details::Context<details::ContextTraits<ClientType, ListenerHandlerType, ApiDefType>>;

}//nbdl

#endif
