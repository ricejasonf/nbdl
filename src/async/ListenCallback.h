#ifndef LISTENCALLBACK_H
#define LISTENCALLBACK_H

class ListenCallbackBase
{
	virtual void notify(PathNodeBase &pathNode, Binder &diff) = 0;
};

template<typename F>
class ListenCallback
{
	F fn;

	public:

	ListenCallBack(F &&fn) :
		fn(std::forward<F>(fn)) {}

	void notify(PathNodeBase pathNode, Binder diff)
	{
		fn(pathNode, diff);
	}
	//todo finish this
};

#endif
