//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_OPTIONAL_HPP
#define NBDL_OPTIONAL_HPP

#include<type_traits>

namespace nbdl {

//todo refactor optional types

template<typename T, template<typename> class TT>
struct IsSpecializationOf : std::false_type { };

template<typename T, template<typename> class TT>
struct IsSpecializationOf<TT<T>, TT> : std::true_type { };

template<typename T, class Enable = void>
struct IsOptional : std::false_type {};

template<typename T, typename = void, class = void>
struct EnableIfOptional;
template<typename T, typename R>
struct EnableIfOptional<T, R, typename std::enable_if<IsOptional<T>::value>::type>
{
	using type = R;
};

}//nbdl

#endif
