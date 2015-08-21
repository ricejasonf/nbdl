#ifndef NBDL_STORE_HASH_MAP_HPP
#define NBDL_STORE_HASH_MAP_HPP

#include<unordered_map>
#include "../Variant.hpp"

namespace nbdl {
namespace store {

template<typename PathType>
class HashMap
{
	public:

	using VariantType = Variant<typename PathType::Entity, NotFound>;

	private:

	using HashFn = typename PathType::HashFn;
	using PredFn = typename PathType::PredFn;
	using Container = std::unordered_map<PathType, VariantType, HashFn, PredFn>;
	Container map;

	public:

	template<typename T>
	void assign(const PathType path, const T value)
	{
		map[path] = value;
	}

	bool hasEntry(const PathType path)
	{
		auto iter = map.find(path);
		return iter != map.end();
	}

	VariantType& get(const PathType path)
	{
		return map[path];
	}
};

}//store
}//nbdl

#endif
