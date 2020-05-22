//
// Copyright Jason Rice 2015-2019
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MPDEF_DIRECTIVE_HPP
#define NBDL_MPDEF_DIRECTIVE_HPP

#include <mpdef/list.hpp>
#include <mpdef/map.hpp>
#include <mpdef/tree_node.hpp>
#include <mpdef/utility.hpp>

#include <boost/hana.hpp>
#include <boost/hana/experimental/types.hpp>

#define MPDEF_DIRECTIVE(NAME)                           \
namespace tag {                                         \
  struct NAME##_t {};                                   \
  constexpr auto NAME = boost::hana::type_c<NAME##_t>;  \
}                                                       \
heavy_macro NAME(...x)  = \
  mpdef::make_tree_node(tag::NAME, \
    mpdef::make_map(mpdef::to_constant(x)...));

#define MPDEF_DIRECTIVE_LIST(NAME)                      \
namespace tag {                                         \
  struct NAME##_t {};                                   \
  constexpr auto NAME = boost::hana::type_c<NAME##_t>;  \
}                                                       \
heavy_macro NAME(...x) =                                \
  mpdef::tree_node<decltype(tag::NAME),                 \
    mpdef::list<decltype(mpdef::to_constant(x))...>>{};

#define MPDEF_DIRECTIVE_LEAF(NAME)                      \
namespace tag {                                         \
  struct NAME##_t {};                                   \
  constexpr auto NAME = boost::hana::type_c<NAME##_t>;  \
}                                                       \
heavy_macro NAME(x) =                              \
  mpdef::tree_node<decltype(tag::NAME),          \
    decltype(mpdef::to_constant(x))>{};

#define MPDEF_DIRECTIVE_TYPE(NAME)                      \
namespace tag {                                         \
  struct NAME##_t {};                                   \
  constexpr auto NAME = boost::hana::type_c<NAME##_t>;  \
}                                                       \
template<typename T>                                    \
constexpr auto NAME = mpdef::make_tree_node(tag::NAME,  \
  boost::hana::type_c<T>);

#define MPDEF_DIRECTIVE_TYPE_LIST(NAME)                 \
namespace tag {                                         \
  struct NAME##_t {};                                   \
  constexpr auto NAME = boost::hana::type_c<NAME##_t>;  \
}                                                       \
template<typename ...T>                                 \
constexpr auto NAME = mpdef::make_tree_node(tag::NAME,  \
  boost::hana::experimental::types<T...>{});

#endif
