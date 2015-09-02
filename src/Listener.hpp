//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_LISTENER_HPP
#define NBDL_LISTENER_HPP

namespace nbdl {

template<typename PathType, typename Context, typename Handler>
class Listener
{
	using ContextWeakPtr = typename Context::WeakPtr;

	PathType path;
	ContextWeakPtr context; //needs to delete self with context
	Handler handler;

	public:
	
	Listener(PathType p, ContextWeakPtr c, Handler h) :
		path(p),
		context(c),
		handler(h)
	{}

	~Listener()
	{
		auto ctx = context.lock(); 
		if (ctx)
		{
			ctx->removeListener(listener.getPath(), *this);
		}
	}

	bool operator== (const Handler& h)
	{
		//handler is stored with the store's emitter
		return handler == h;
	}

	//todo delete self from context
	//todo == operator needed for key comparison

	template<typename... Args>
	void notify(Args... args)
	{
		handler.notify(args...);
	}

};

/*
   this EventHandler just calls a 
   dummy callback on any kind of 
   state change with no args
*/
template<typename WeakPtr, typename Fn = std::function<void(typename WeakPtr::element_type&)>>
class EventHandlerDummy : public EventHandlerBase<EventHandlerDummy<WeakPtr>>
{
	using ElementType = typename WeakPtr::element_type;

	Fn fn;

	EventHandlerDummy(WeakPtr w, Fn f) :
		EventHandlerBase<EventHandlerDummy<WeakPtr>>(w),
		fn(f)
	{}

	template<typename... Args>
	void notify(ElementType& element, Args... args)
	{
		fn(element);
	}
};

template<typename WeakPtr, typename Impl>
class EventHandlerBase
{
	//weak could be a socket connection or just 
	//an interal handle for a ui object instance
	WeakPtr weak;

	public:
	
	EventHandlerBase(WeakPtr w) :
	{}

	bool operator== (const Handler& h)
	{
		//to find in a vector or other lookup
		!weak.owner_before(h.weak) && !h.weak.owner_before(weak);
	}

	template<typename... Args>
	void notify_(Args... args)
	{
		auto shared = weak.lock();
		if (shared)
		{
			static_cast<Impl*>(this)->notify(*shared, args);
		}
	}
};

}//nbdl

#endif
