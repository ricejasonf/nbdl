//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_STORE_HASH_MAP_HPP
#define NBDL_STORE_HASH_MAP_HPP

#include<Variant.hpp>

#include<unordered_map>

namespace nbdl {

struct NotFound;

namespace store {

template<typename Path_>
class HashMap
{
	public:

	using Variant_ = Variant<typename Path_::Entity, NotFound>;

	private:

	using HashFn = typename Path_::HashFn;
	using PredFn = typename Path_::PredFn;
	using Container = std::unordered_map<Path_, Variant_, HashFn, PredFn>;
	Container map;

	public:

	template<typename T>
	const Variant_& assign(const Path_& path, T&& value)
	{
		return map[path] = std::forward<T>(value);
	}

	bool hasEntry(const Path_& path)
	{
		auto iter = map.find(path);
		return iter != map.end();
	}

	Variant_& get(const Path_& path)
	{
		return map[path];
	}
};

}//store
}//nbdl

#endif
