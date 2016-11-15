//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DETAIL_COMMON_TYPE_HPP
#define NBDL_DETAIL_COMMON_TYPE_HPP

#include<type_traits>

namespace nbdl {
namespace detail {

// fixes bug in Xcodes outdated version of libc++ :(
template<typename ...T>
struct common_type
{
  using type = typename std::common_type<T...>::type;
};

template<>
struct common_type<void, void>
{
  using type = void;
};

template<typename ...T>
using common_type_t = typename nbdl::detail::common_type<T...>::type;

} // detail
} // nbdl

#endif
