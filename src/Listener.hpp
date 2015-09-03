//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_LISTENER_HPP
#define NBDL_LISTENER_HPP

#include<functional>

namespace nbdl {

template<typename WeakPtrType, typename Impl>
class ListenerHandlerBase
{
	using WeakPtr = WeakPtrType;
	//weak could be a socket connection or just 
	//an interal handle for a ui object instance
	WeakPtr weak;

	public:
	
	ListenerHandlerBase(WeakPtr w) :
		weak(w)
	{}

	bool operator== (const Impl& h)
	{
		//to find in a vector or other lookup
		return !weak.owner_before(h.weak) && !h.weak.owner_before(weak);
	}

	template<typename VariantType>
	void notify_(const VariantType& value)
	{
		if (!weak.expired())
		{
			static_cast<Impl*>(this)->notify(value);
		}
	}
};

/*
   this ListenerHandler just calls a 
   dummy callback on any kind of 
   state change with no args
*/
template<typename WeakPtrType, typename Fn = std::function<void()>>
class ListenerHandlerDummy : public ListenerHandlerBase<WeakPtrType, ListenerHandlerDummy<WeakPtrType, Fn>>
{
	using ElementType = typename WeakPtrType::element_type;

	friend class ListenerHandlerBase<WeakPtrType, ListenerHandlerDummy<WeakPtrType, Fn>>;

	Fn fn;

	template<typename VariantType>
	void notify(const VariantType&)
	{
		fn();
	}

	public:

	ListenerHandlerDummy(WeakPtrType w, Fn f) :
		ListenerHandlerBase<WeakPtrType, ListenerHandlerDummy<WeakPtrType, Fn>>(w),
		fn(f)
	{}

	using WeakPtr = WeakPtrType;
};

namespace details {

template<typename Context, typename PathType>
class Listener
{
	using ContextWeakPtr = typename Context::WeakPtr;
	using Handler = typename Context::ListenerHandler;

	PathType path;
	ContextWeakPtr context; //needs to delete self with context
	Handler handler;

	public:
	
	Listener(PathType p, ContextWeakPtr c, Handler h) :
		path(p),
		context(c),
		handler(h)
	{
		//perhaps i should add the listener here?
	}

	~Listener()
	{
		auto ctx = context.lock(); 
		if (ctx)
		{
			ctx->removeListener(path, handler);
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

}//details

}//nbdl

#endif
