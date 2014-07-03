#ifndef LISTENREQUEST_H
#define LISTENREQUEST_H

#include<memory>

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
	std::unique_ptr<ListenCallbackBase> _notify;

	public:

	ListenRequest(std::unique_ptr<PathNode> p) :
		ListenRequestBase(p) {}

	template<typename F>
	ListenRequest &notify(F&& fn)
	{
		_notify = std::unique_ptr<ListenCallbackBase>(
				new ListenCallback(std::forward<F>(fn)));	
		return *this;
	}

	void callNotify(PathNode &pathNode, Binder &binder)
	{
		_notify->notify(pathNode, binder);
	}

	T::Ref getEntityRef()
	{
		return Entity::Ref(entity);
	}
};

#endif
