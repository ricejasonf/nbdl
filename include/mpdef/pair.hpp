//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MPDEF_PAIR_HPP
#define NBDL_MPDEF_PAIR_HPP

#include <mpdef/tree_node.hpp>

namespace mpdef {

template<typename First, typename Second>
using pair = mpdef::tree_node<First, Second>;

constexpr auto make_pair = mpdef::make_tree_node;

}//mpdef

#endif

