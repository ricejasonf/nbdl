//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_LISTENER_HPP
#define NBDL_LISTENER_HPP

namespace nbdl {

template<typename ContextPtr, typename StoreType, typename Fn>
class Listener
{
	using Variant = StoreType::Variant;

	ContextPtr context; //needs to delete self with context
	Fn fn;

	public:
	
	Listener(ContextPtr c, Fn f) :
		context(c),
		fn(fn)
	{}
	//todo delete self from context
	//todo == operator needed for key comparison

	//'Change' specifically means a change 
	//to the variants internal type
	onChange()
	{
		fn();
	}

	/* todo create EntityDiff but 
	   really this Listener is to 
	   dumb to do anything with it

	onDiff(EntityDiff)
	{
		fn();
	}
	*/

};

}//nbdl

#endif
