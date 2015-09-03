//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_STORE_EMITTER_HASH_MAP_HPP
#define NBDL_STORE_EMITTER_HASH_MAP_HPP

#include<algorithm>
#include<vector>
#include<unordered_map>

namespace nbdl {
namespace store_emitter {

template<typename PathType, typename ListenerHandlerType>
class HashMap
{
	public:

	using VariantType = Variant<typename PathType::Entity, NotFound>;
	using ListenerHandler = ListenerHandlerType;

	private:

	using HashFn = typename PathType::HashFn;
	using PredFn = typename PathType::PredFn;
	using Vector = std::vector<ListenerHandler>;
	using Container = std::unordered_map<PathType, Vector, HashFn, PredFn>;
	Container map;

	public:

	template<typename Fn>
	void emit(const PathType& path, Fn fn)
	{
		auto map_node = map.find(path);
		if (map_node != map.end())
		{
			Vector vector = map_node->second;
			for (ListenerHandler listener : vector)
			{
				fn(listener);
			}
		}
	}

	void addListener(const PathType& path, ListenerHandler listener)
	{
		map[path].push_back(listener);
	}

	void removeListener(const PathType& path, const ListenerHandler& listener)
	{
		auto map_node = map.find(path);
		if (map_node != map.end())
		{
			Vector vector = map_node->second;
			vector.erase(std::find(vector.begin(), vector.end(), listener));	
			if (!vector.size())
			{
				map.erase(path);
				return;
			}
		};
	}
};

}//store_emitter
}//nbdl

#endif
