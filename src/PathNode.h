#ifndef PATHNODE_H
#define PATHNODE_H

#include<string>
#include<functional>
#include<type_traits>
#include "Entity.h"

class PathNodeBase
{
	int id;

	public:

	PathNodeBase(int id = 0) : id(id) {}

	PathNodeBase &getParent() = 0;
	const std::string getName() = 0;
	int getId() { return id; }

}

template<class T>
class PathNode
{
	static_assert(std::is_base_of<Entity, T>::value, "T must be derived from Entity");

	public:

	PathNode(int id = 0) : PathNodeBase(id) {}

	T create() { return T(); }
	RetrieveCallback<T> retrieve() 
		{ return RetrieveCallback<T>(this); }
	ListenCallback<T, ValueMap> listen(std::function<T, ValueMap> fn) 
		{ return ListenCallback<T, ValueMap>(this, fn); }
	ListenCallback<T, ValueMap, ValueMap> listen(std::function<T, ValueMap, ValueMap> fn) 
		{ return ListenCallback<T, ValueMap, ValueMap>(this, fn); }
	ListenCallback<T, ValueMap, ValueMap> listen(std::function<T, ValueMap, ValueMap> fn) 
		{ return ListenCallback<T, ValueMap, ValueMap>(this, fn); }
	/* save needs to be in the root node
	SaveCallback<T> save(T entity) 
		{ return SaveCallback<T>(this, entity); }
		*/
}

/*
	get path string from fully built path
	revert to parent node with concrete type using accessor must be implemented by client
	the following is a capability but it is not limited to this:
	the entity for every node in the path will be loaded an checked against the acl
	so really deep trees is a bad idea unless parent entities are cached on the machine
*/

#endif
