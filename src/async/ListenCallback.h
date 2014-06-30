#ifndef LISTENREQUESTCALLBACK_H
#define LISTENREQUESTCALLBACK_H

class ListenRequestCallbackBase
{
	virtual void notify(PathNodeBase &pathNode, Binder &diff) = 0;
};

template<typename F>
class ListenRequestCallback
{
	F fn;

	void notify(PathNodeBase &pathNode, Binder &diff)
	{
		fn(pathNode, diff);
	}
	//todo finish this
};

#endif
