#ifndef NBDL_ACCESS_POINT_HPP
#define NBDL_ACCESS_POINT_HPP

#include<tuple>
#include<boost/functional/hash.hpp>

namespace nbdl {

namespace {

//todo break this out and give it a start and a stop
template<int ...>
struct NumberSequence {};
template<int n, int... sequence>
struct GenerateNumberSequence
{
	using Type = typename GenerateNumberSequence<n - 1, n - 1, sequence...>::Type;
};
template<int... sequence>
struct GenerateNumberSequence<1, sequence...>
{
	using Type = NumberSequence<sequence...>;
};

//Hash Function
template<typename AccessPointType>
struct HashAccessPointFn
{
	using Tuple = typename AccessPointType::Tuple;
	using Hasher = typename AccessPointType::template Hasher<0, Tuple>;

	std::size_t operator() (const AccessPointType& path) const
	{
		std::size_t seed;
		Hasher::call(seed, path.tuple);
		return seed;
	}
};

template<typename AccessPointType>
struct PredAccessPointFn
{
	constexpr bool operator() (const AccessPointType& t1, const AccessPointType& t2)
	{
		return t1.tuple == t2.tuple;
	}
};

}//anon

template<typename EntityType, typename KeyType, typename ParentAccessPoint = void>
class AccessPoint
{
	public:

	using ParentTuple = typename ParentAccessPoint::Tuple;
	using Tuple = decltype(std::tuple_cat(std::declval<std::tuple<KeyType>>(), std::declval<ParentTuple>()));
	using Key = KeyType;
	using Entity = EntityType;
	using HashFn = HashAccessPointFn<AccessPoint>;
	using PredFn = PredAccessPointFn<AccessPoint>;

	Tuple tuple;

	template<int... sequence>
	ParentTuple makeParentTuple(NumberSequence<sequence...>)
	{
		return std::make_tuple(std::get<sequence>(tuple)...);
	}

	AccessPoint(Key k, ParentAccessPoint p) : 
		tuple(std::tuple_cat(std::make_tuple(k), p.tuple))
	{}

	AccessPoint(Tuple t) :
		tuple(t)
	{}

	KeyType getKey()
	{
		return std::get<0>(tuple);
	}

	ParentAccessPoint getParent()
	{
		return ParentAccessPoint(makeParentTuple(typename GenerateNumberSequence<std::tuple_size<Tuple>::value>::Type()));
	}

	template<int i, typename T>
	struct Hasher
	{
		using ParentHasher = typename ParentAccessPoint::template Hasher<(i + 1), T>;
		static void call(std::size_t& seed, const T& tuple)
		{
			boost::hash_combine(seed, std::get<i>(tuple));	
			ParentHasher::call(seed, tuple);
		}
	};
};

template<typename EntityType, typename KeyType>
class AccessPoint<EntityType, KeyType, void>
{
	public:

	using Tuple = std::tuple<KeyType>;
	using Key = KeyType;
	using Entity = EntityType;
	using HashFn = HashAccessPointFn<AccessPoint>;
	using PredFn = PredAccessPointFn<AccessPoint>;

	Tuple tuple;

	AccessPoint(Key k) : 
		tuple(std::make_tuple(k)) 
	{}

	AccessPoint(Tuple t) :
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

template<typename AccessPoint>
struct AccessPointName {};
}//nbdl

#endif
