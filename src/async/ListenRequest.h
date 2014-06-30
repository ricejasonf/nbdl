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
};

template<class T>
class ListenRequest
{
	//todo use ListenCallbackBase
	std::function<void(ValueMap, ValueMap)> _notify;

	public:

	ListenRequest(std::unique_ptr<PathNode> p) :
		ListenRequestBase(p) {}

	ListenRequest &notify(std::function<void(ValueMap, ValueMap)> fn)
	{
		_notify = fn;	
		return *this;
	}

	void callNotify(ValueMap diff, ValueMap keys) 
	{
		_notify(diff, keys);
	}

	T::Ref getEntityRef()
	{
		return Entity::Ref(entity);
	}
};

#endif
