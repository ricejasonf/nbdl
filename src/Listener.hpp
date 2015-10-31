//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_LISTENER_HPP
#define NBDL_LISTENER_HPP

#include<memory>
#include<functional>

namespace nbdl {

using ListenerHandle = std::weak_ptr<int>;

template<typename Impl>
class ListenerHandlerBase
{
	ListenerHandle weak;

	public:
	
	ListenerHandlerBase(ListenerHandle w) :
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
template<typename Fn = std::function<void()>>
class ListenerHandlerDummy : public ListenerHandlerBase<ListenerHandlerDummy<Fn>>
{
	using ElementType = typename ListenerHandle::element_type;

	friend class ListenerHandlerBase<ListenerHandlerDummy<Fn>>;

	Fn fn;

	template<typename VariantType>
	void notify(const VariantType&)
	{
		fn();
	}

	public:

	ListenerHandlerDummy(ListenerHandle w, Fn f) :
		ListenerHandlerBase<ListenerHandlerDummy<Fn>>(w),
		fn(f)
	{}
};

namespace details {

template<typename Handler, typename ContextWeakPtr, typename PathType>
class Listener
{
	PathType path;
	ContextWeakPtr context; //needs to delete self with context
	std::shared_ptr<int> handle_master;
	Handler handler_;

	public:
	
	template<typename... Args>
	Listener(PathType p, ContextWeakPtr c, Args... args) :
		path(p),
		context(c),
		handle_master(std::make_shared<int>(0)),
		handler_(Handler(handle_master, args...))
	{}
	//todo copy constructor needs to be deleted or something

	~Listener()
	{
		auto ctx = context.lock(); 
		if (ctx)
		{
			ctx->removeListener(path, handler_);
		}
	}

	bool operator== (const Handler& h)
	{
		//handler_ is stored with the store's emitter
		return handler_ == h;
	}

	Handler handler()
	{
		return handler_;
	}

	//todo delete self from context
	//todo == operator needed for key comparison

	template<typename... Args>
	void notify(Args... args)
	{
		handler_.notify(args...);
	}

};

}//details

}//nbdl

#endif
