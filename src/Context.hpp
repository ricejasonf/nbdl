//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONTEXT_HPP
#define NBDL_CONTEXT_HPP

#include<memory>

namespace nbdl {

namespace details {

template<typename Traits>
class Context : public std::enable_shared_from_this<Context<Traits>>
{
  using ProviderMap = typename Traits::ProviderMap;
  using StoreMap = typename Traits::StoreMap;
  using Consumers = typename Traits::Consumers;
  using ListenerHandler_ = nbdl::ListenerHandlerDummy<>;

	public:

	using SharedPtr = std::shared_ptr<Context>;
	using WeakPtr = std::weak_ptr<Context>;

  template<typename PathType>
	using Listener = details::Listener<ListenerHandler_, WeakPtr, PathType>;

	private:

  ProviderMap providers;
	StoreMap stores;

	public:

	template<typename PathType, typename... Args>
	Listener<PathType> makeListener(PathType path, Args... args)
	{
		auto wrapper = Listener<PathType>(path, this->shared_from_this(), args...);
		stores[hana::type_c<PathType>].addListener(path, wrapper.handler());
		return wrapper;
	}
	template<typename PathType>
	void addListener(const PathType& path, const ListenerHandler_& listener)
	{
		stores[hana::type_c<PathType>].addListener(path, listener);
	}
	template<typename PathType>
	void removeListener(const PathType& path, const ListenerHandler_& listener)
	{
		stores[hana::type_c<PathType>].removeListener(path, listener);
	}

	template<typename Path,typename... MatchFns>
	auto read(Path path, MatchFns... fns)
	{
		return stores[hana::type_c<Path>].get(
			//called if store needs to request value
			[&](const Path& path) {
				//request from provider
				auto self = this->shared_from_this();
				providers[hana::type_c<Path>].read(path, [path, self](auto&& value) {
					self->stores[hana::type_c<Path>]
            .suggestAssign(path, std::forward<decltype(value)>(value));
				});
			},
			path, fns...);
	}
};

template<
	typename ProviderMap_,
  typename Consumers_,
	typename StoreMap_>
struct ContextTraits
{
	using ProviderMap = ProviderMap_;
	using Consumers = Consumers_;
	using StoreMap = StoreMap_;
};


}//details

template<typename Traits>
struct Context
{
	using Type = details::Context<Traits>;
	using SharedPtr = typename Type::SharedPtr;

	//todo i could probably wrap the shared object and put the logic in here
	//and eliminate the shared_from_this junk
	template<typename... Args>
	static SharedPtr create(Args... args)
	{
		return std::make_shared<Type>(args...);
	}
};

}//nbdl

#endif
