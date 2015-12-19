//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_MAKE_HPP
#define NBDL_DEF_BUILDER_MAKE_HPP

#include "../builder.hpp"

namespace nbdl_def {
namespace builder {

//make for building builders from defs (ie pair(tag, tuple_of_children))
template<template<typename> typename Builder_>
struct Make
{
  template<typename Def>
  constexpr auto operator()(Def def)
  {
    using DefPath = hana::tuple<Def>;
    using Builder = Builder_<DefPath>;
    static_assert(hana::type_c<typename Builder::Tag> == hana::type_c<decltype(hana::first(def))>,
      "Attempt to construct Builder with invalid definition (ie Tag)");
    return Builder();
  }

  template<typename Def, typename ParentDefs>
  constexpr auto operator()(Def def, ParentDefs parent_defs)
  {
    using DefPath = (decltype(hana::prepend(def, parent_defs)));
    using Builder = Builder_<DefPath>;
    static_assert(hana::type_c<typename Builder::Tag> == hana::type_c<decltype(hana::first(def))>,
      "Attempt to construct Builder with invalid definition (ie Tag)");
    return Builder();
  }

  template<typename ParentDefs, typename... Defs>
  constexpr auto operator()(hana::tuple<Defs> defs, ParentDefs parent_defs)
  {
    return hana::transform(defs, this);
  }
};

template<template<typename> typename Builder>
constexpr Make<Builder> make{};

/* example usage:
 *  make<AccessPoint>(def, parent_defs);
 */

}//builder
}//nbdl
#endif
