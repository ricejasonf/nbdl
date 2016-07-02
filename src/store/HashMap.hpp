//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_STORE_HASH_MAP_HPP
#define NBDL_STORE_HASH_MAP_HPP

#include<nbdl/variant.hpp>

#include<unordered_map>

namespace nbdl {

struct not_found;

namespace store_container {

template<typename Path_>
class hash_map
{
	public:

	using Variant_ = variant<typename Path_::Entity, not_found>;

	private:

	using HashFn = typename Path_::hash_fn;
	using PredFn = typename Path_::pred_fn;
	using Container = std::unordered_map<Path_, Variant_, HashFn, PredFn>;
	Container map;

	public:

  hash_map()
    : map()
  { }

	template<typename T>
	const Variant_& assign(const Path_& path, T&& value)
	{
		return map[path] = std::forward<T>(value);
	}

	bool has_entry(const Path_& path)
	{
		auto iter = map.find(path);
		return iter != map.end();
	}

	Variant_& get(const Path_& path)
	{
		return map[path];
	}
};

}//store_container
}//nbdl

#endif
