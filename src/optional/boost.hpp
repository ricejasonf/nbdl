//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_OPTIONAL_BOOST_HPP
#define NBDL_OPTIONAL_BOOST_HPP

#include<boost/optional.hpp>

/*
 * recognize boost::optional as an optional value
 */

namespace nbdl {

template<typename T>
struct IsOptional<T, 
	typename std::enable_if<IsSpecializationOf<T, boost::optional>::value>::type>
	: std::true_type {};

}//nbdl

#endif
