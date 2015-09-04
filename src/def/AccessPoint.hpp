//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBLD_DEF_ACCESS_POINT_HPP
#define NBLD_DEF_ACCESS_POINT_HPP

#include "Path.hpp"

namespace nbdl_def {

//import nbdl::CreatePath for convenience and readability
template<typename... Ts>
using Path = nbdl::CreatePath<Ts...>;

template<typename PathType, typename ActionsType>
struct AccessPoint
{
	using Path = PathType;
	using Actions = ActionsType;
};

}//nbdl_def

#endif
