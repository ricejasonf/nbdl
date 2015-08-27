//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BINDER_HPP
#define BINDER_HPP

namespace nbdl {

template<class Impl>
class Binder
{
	public:

	template<typename T>
	void bindMember(const std::string name, T &field)
	{
		static_cast<Impl*>(this)->bind(name, field);
	}
};

}//nbdl

#endif
