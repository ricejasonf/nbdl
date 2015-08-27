#ifndef NBDL_PATH_HPP
#define NBDL_PATH_HPP

#include<type_traits>
#include<tuple>
#include<boost/functional/hash.hpp>

#include "mpl/Reverse.hpp"
#include "EntityTraits.hpp"
#include "EntityKey.hpp"

namespace nbdl {

namespace path_details {

//todo break this out and give it a start and a stop
template<int ...>
struct NumberSequence {};
template<int n, int... sequence>
struct GenerateNumberSequence
{
	using Type = typename GenerateNumberSequence<n - 1, n - 1, sequence...>::Type;
};
template<int... sequence>
struct GenerateNumberSequence<0, sequence...>
{
	using Type = NumberSequence<sequence...>;
};

//Hash Function
template<typename PathType>
struct HashPathFn
{
	using Tuple = typename PathType::Tuple;
	using Hasher = typename PathType::template Hasher<0, Tuple>;

	std::size_t operator() (const PathType& path) const
	{
		std::size_t seed;
		Hasher::call(seed, path.tuple);
		return seed;
	}
};

template<typename PathType>
struct PredPathFn
{
	bool operator() (const PathType& t1, const PathType& t2) const
	{
		return t1.tuple == t2.tuple;
	}
};

template<typename PathType, typename EntityType, int i, class = void>
struct FindPath
{
	using Parent = typename PathType::Parent;
	static const int value = FindPath<Parent, EntityType, (i - 1)>::value;
};
template<typename PathType, typename EntityType, int i>
struct FindPath<PathType, EntityType, i,
	typename std::enable_if<
		std::is_same<typename PathType::Entity, EntityType>::value
	>::type>
{
	static const int value = i;
};

}//path_details

template<typename EntityType, typename KeyType, typename ParentPath = void>
class Path
{
	public:

	using Parent = ParentPath;
	using ParentTuple = typename ParentPath::Tuple;
	using ParentKey = typename ParentPath::Key;
	using Tuple = decltype(std::tuple_cat(std::declval<ParentTuple>(), std::declval<std::tuple<KeyType>>()));
	using Key = KeyType;
	using Entity = EntityType;
	using HashFn = path_details::HashPathFn<Path>;
	using PredFn = path_details::PredPathFn<Path>;
	template<typename E, int i = std::tuple_size<Tuple>::value - 1>
	using FindPath = path_details::FindPath<Path, E, i>;

	Tuple tuple;

	private:

	template<int... sequence>
	ParentTuple makeParentTuple(path_details::NumberSequence<sequence...>)
	{
		return std::make_tuple(std::get<sequence>(tuple)...);
	}

	public:

	Path(Key k, ParentPath p) : 
		tuple(std::tuple_cat(p.tuple, std::make_tuple(k)))
	{}

	Path(Tuple t) :
		tuple(t)
	{}

	template<typename... Args>
	Path(Args... args) :
		tuple(std::make_tuple(args...))
	{}

	template<typename E = EntityType>
	KeyType getKey()
	{
		return std::get<FindPath<E>::value>(tuple);
	}

	ParentPath getParent()
	{
		return ParentPath(makeParentTuple(typename path_details::GenerateNumberSequence<(std::tuple_size<Tuple>::value - 1)>::Type()));
	}

	template<int i, typename T>
	struct Hasher
	{
		using ParentHasher = typename ParentPath::template Hasher<(i + 1), T>;
		static void call(std::size_t& seed, const T& tuple)
		{
			boost::hash_combine(seed, std::get<i>(tuple));	
			ParentHasher::call(seed, tuple);
		}
	};
};

template<typename EntityType, typename KeyType>
class Path<EntityType, KeyType, void>
{
	public:

	using Tuple = std::tuple<KeyType>;
	using Key = KeyType;
	using Entity = EntityType;
	using HashFn = path_details::HashPathFn<Path>;
	using PredFn = path_details::PredPathFn<Path>;

	Tuple tuple;

	Path(Key k) : 
		tuple(std::make_tuple(k)) 
	{}

	Path(Tuple t) :
		tuple(t)
	{}

	KeyType getKey()
	{
		return std::get<0>(tuple);
	}

	template<int i, typename T>
	struct Hasher
	{
		static void call(std::size_t& seed, const T& tuple)
		{
			boost::hash_combine(seed, std::get<i>(tuple));	
		}
	};
};

template<typename Path>
struct PathName {};

template<typename PathType, class = void>
struct CreatePathNode
{
	using Type = PathType;
};
template<typename EntityType>
struct CreatePathNode<EntityType,
	typename std::enable_if<IsEntity<EntityType>::value>::type>
{
	using Type = Path<EntityType, typename PrimaryKey<EntityType>::MemberType>;
};

template<typename Tuple>
struct CreatePathFromReversedTuple;

template<typename... Args>
struct CreatePathFromReversedTuple<std::tuple<Args...>>
{
	using Type = void;
};

template<typename PathType, typename... Args>
struct CreatePathFromReversedTuple<std::tuple<PathType, Args...>>
{
	using PathNode = typename CreatePathNode<PathType>::Type;
	using Type = Path<typename PathNode::Entity, typename PathNode::Key, typename CreatePathFromReversedTuple<std::tuple<Args...>>::Type>;
};

template<typename... Args>
struct CreatePath
{
	using ReversedTuple = typename mpl::Reverse<std::tuple<Args...>>::Type;
	using Type = typename CreatePathFromReversedTuple<ReversedTuple>::Type;	
};

}//nbdl

#endif
