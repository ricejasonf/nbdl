#ifndef NBDL_ACCESS_POINT_HPP
#define NBDL_ACCESS_POINT_HPP

#include<tuple>

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

}//anon

template<typename EntityType, typename KeyType, typename ParentAccessPoint = void>
class AccessPoint
{
	public:

	using ParentTuple = typename ParentAccessPoint::Tuple;
	using Tuple = decltype(std::tuple_cat(std::declval<std::tuple<KeyType>>(), std::declval<ParentTuple>()));
	using Key = KeyType;
	using Entity = EntityType;

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
};

template<typename EntityType, typename KeyType>
class AccessPoint<EntityType, KeyType, void>
{
	public:

	using Tuple = std::tuple<KeyType>;
	using Key = KeyType;
	using Entity = EntityType;

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

};

template<typename AccessPoint>
struct AccessPointName {};

}//nbdl

namespace std {

template<typename EntityType, typename KeyType, typename ParentAccessPoint>
struct hash<nbdl::AccessPoint<EntityType, KeyType, ParentAccessPoint>>
{
	using AccessPoint = nbdl::AccessPoint<EntityType, KeyType, ParentAccessPoint>;
	using Tuple = typename AccessPoint::Tuple;

	std::size_t operator()(const AccessPoint& path) const
	{
		return std::hash<Tuple>(path.tuple);
	}
};

}//std

#endif
