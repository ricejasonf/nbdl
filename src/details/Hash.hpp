//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_HASH_HPP
#define NBDL_HASH_HPP

#include <functional>

namespace nbdl {
namespace details {

// sort of excerpted from boost with the same license 
// and the following copyright
// Copyright 2005-2009 Daniel James.
template <class T>
void hash_combine(std::size_t& seed, T const& v)
{
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

}//details
}//nbdl

#endif
