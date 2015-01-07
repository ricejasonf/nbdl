#ifndef PATHNODE_H
#define PATHNODE_H

#include<stdint.h>
#include<string>
#include "Entity.hpp"

template<KeyType = uint32_t>
struct Id
{
	KeyType id;
	std::string localId;

	Id(KeyType id = 0, localId = 0) :
		id(id),
		localId(localId) {}
}

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

template<class Impl, class Entity, class ParentPathNode, typename KeyType = Id>
class PathNode
{
	ParentPathNode parent_;
	KeyType id_;

	public:

	PathNode(KeyType id) : id_(id) {}

	KeyType id() { return id_; }
	ParentPathNode parent() { return parent_; }

};

class ClientPathNode : public PathNode<ClientPathNode, Client>
{
	void register()
	{
		RootPath::registerChild(*this);
	}
};
class PropertyPathNode : public PathNode<PropertyPathNode, Property, ClientPathNode>
{
	const char *parentKeyName()
	{
		return "clientId";
	}
};

#endif
