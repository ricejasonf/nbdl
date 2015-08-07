#ifndef NBDL_ACCESS_POINT_HPP
#define NBDL_ACCESS_POINT_HPP

namespace nbdl {

template<typename EntityType, typename KeyType, typename ParentAccessPoint = void>
struct AccessPoint
{
	using Entity = EntityType;
	using Key = KeyType;

	AccessPoint(Key k, ParentAccessPoint p) : 
		parent(p),
		key(k) {}
	ParentAccessPoint parent;
	Key key;
};

template<typename EntityType, typename KeyType>
struct AccessPoint<EntityType, KeyType, void>
{
	using Entity = EntityType;
	using Key = KeyType;

	AccessPoint(Key k) : 
		key(k) {}	
	Key key;
};

template<typename AccessPoint>
struct AccessPointName {};

}//nbdl

#endif
