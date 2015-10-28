//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_DEFINE_API_DEFINITION_HPP
#define NBDL_DEF_DEFINE_API_DEFINITION_HPP

#include<type_traits>
#include<boost/hana.hpp>

#define NBDL_DEF_DIRECTIVE(NAME) \
namespace tag { constexpr auto NAME = BOOST_HANA_STRING(#NAME); } \
template<typename... Tn> constexpr auto NAME(Tn... tn) \
{ return hana::make_tuple(tag::NAME, tn...); }

namespace nbdl_def {

NBDL_DEF_DIRECTIVE(Type)
NBDL_DEF_DIRECTIVE(ListItem)
NBDL_DEF_DIRECTIVE(DefineApi)
NBDL_DEF_DIRECTIVE(Entities)
NBDL_DEF_DIRECTIVE(ImportApi)
NBDL_DEF_DIRECTIVE(ImportExternalApi)
NBDL_DEF_DIRECTIVE(AccessPoint)
NBDL_DEF_DIRECTIVE(Path)
NBDL_DEF_DIRECTIVE(Actions)

constexpr bool tagMatch = hana::equal.to ^hana::on^ hana::reverse_partial(hana::at, 0);

template<typename Xs, Tag>
constexpr auto getByTag(Xs xs, Tag tag)
{
  return hana::filter(xs, tagMatch(tag));
}

template<typename Xs, Tag>
constexpr auto findByTag(Xs xs, Tag tag)
{
  return hana::find_if(xs, tagMatch(tag));
}

}//nbdl_def

#endif
