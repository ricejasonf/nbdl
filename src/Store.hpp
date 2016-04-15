//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_STORE_HPP
#define NBDL_STORE_HPP

#include "store/HashMap.hpp"
#include <nbdl/fwd/make_store.hpp> // for nbdl::not_found

namespace nbdl {

template<
  typename Container,
  typename PathType >
class store
{
	using Entity = typename PathType::Entity;

	Container container;

	public:

	using Variant_ = typename Container::Variant_;

	template<typename T>
	void force_assign(const PathType& path, T&& value)
	{
		container.assign(path, std::forward<T>(value));
	}

	template<typename T>
	void suggest_assign(const PathType& path, T&& val)
	{
		Variant_& value = container.get(path);
    if (value.template is<unresolved>())
		{
			value = std::forward<T>(val);
		}
	}

	template<typename... MatchFns>
	auto get(const PathType& path, MatchFns... fns)
	{
		return container.get(path).match(fns...);
	}
};

}//nbdl

#endif
