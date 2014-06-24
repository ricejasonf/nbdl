#ifndef LISTENREQUEST_H
#define LISTENREQUEST_H

#include<functional>

class ListenRequestBase
{
	std::unique_ptr<PathNode> pathNode;

	public:

	ListenRequestBase(std::unique_ptr<PathNode> p) :
		pathNode(p) {}
	ListenRequestBase(ListenRequestBase &&) {}
	virtual ~ListenRequestBase();

	virtual void callNotify() = 0;
	virtual Entity::Ref getEntityRef() = 0;
};

template<class T>
class ListenRequest
{
	std::function<void(T)> _notify;
	std::function<void()> _notFound;
	std::function<void()> _fail;
	T entity;

	public:

	ListenRequest(std::unique_ptr<PathNode> p) :
		ListenRequestBase(p) {}

	ListenRequest &notify(std::function<void, T> fn)
	{
		_notify = fn;	
		return *this;
	}

	void callNotify() 
	{
		//todo work in diff or something
		_notify(entity);
	}

	T::Ref getEntityRef()
	{
		return Entity::Ref(entity);
	}
};

#endif
