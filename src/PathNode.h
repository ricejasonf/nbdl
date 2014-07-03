#ifndef PATHNODE_H
#define PATHNODE_H

#include<string>
#include<functional>
#include<type_traits>
#include "Entity.h"

class PathNodeBase
{
	public:

	PathNodeBase(int id = 0) : id(id) {}

	int id;

	PathNodeBase &getParent() = 0;
	const std::string getName() = 0;
	const std::string getKeyName() = 0;
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

	ListenCallback<T, std::reference_wrapper<Binder>> 
		listen(std::function<T, std::reference_wrapper<Binder>> fn) 
		{ return ListenCallback<T, std::reference_wrapper<Binder>>(this, fn); }

	/* save needs to be in the root node
	SaveCallback<T> save(T entity) 
		{ return SaveCallback<T>(this, entity); }
		*/
}

/*
	get path string from fully built path
	revert to parent node with concrete type using accessor must be implemented by client
*/

#endif
