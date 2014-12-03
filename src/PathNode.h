#ifndef PATHNODE_H
#define PATHNODE_H

#include<string>
#include<functional>
#include<type_traits>
#include "Entity.hpp"

class PathNodeBase
{
	public:

	virtual ~PathNodeBase();

	virtual std::string getName() = 0;
	virtual std::string getKeyName() = 0;
	virtual std::string getKey() = 0;
	virtual std::string getPath() { return _getPath(); }

	protected:

	std::string getPath(PathNodeBase &parent) { return parent.getPath() + _getPath(); }

	private:

	std::string _getPath() { return "/" + getName() + "/" + getKey(); } 
};

template<class T, typename KeyType = unsigned>
class PathNode
{
	static_assert(std::is_base_of<EntityPersistent<KeyType>, T>::value, 
			"T must be derived from EntityPersistent<KeyType>");

	public:

	PathNode(KeyType id) : id(id) {}

	KeyType id;
	T create() { return T(id); }

	RetrieveCallback<T> retrieve() 
		{ return RetrieveCallback<T>(*this); }

	ListenCallback<T, std::reference_wrapper<Binder>> 
		listen(std::function<T, std::reference_wrapper<Binder>> fn) 
		{ return ListenCallback<T, std::reference_wrapper<Binder>>(*this, fn); }

	SaveCallback<T> save(T entity) 
		{ return SaveCallback<T>(*this, entity); }
};

#endif
