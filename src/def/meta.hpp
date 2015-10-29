//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DDL_META_DEFINITION_HPP
#define NBDL_DDL_META_DEFINITION_HPP

#include<boost/hana.hpp>

#define NBDL_DDL_DIRECTIVE(NAME) \
namespace tag { \
  constexpr auto NAME = BOOST_HANA_STRING(#NAME); } \
  using NAME_t = decltype(NAME); \
} \
template<typename... Tn> constexpr auto NAME(Tn... tn) \
{ return hana::make_tuple(tag::NAME, tn...); } \

namespace nbdl_ddl_meta {

constexpr bool tagMatch = hana::equal.to ^hana::on^ hana::reverse_partial(hana::at, 0);

template<typename Xs, Tag>
constexpr auto filterByTag(Xs xs, Tag tag)
{
  return hana::filter(xs, tagMatch(tag));
}

template<typename Xs, Tag>
constexpr auto findByTag(Xs xs, Tag tag)
{
  return hana::find_if(xs, tagMatch(tag));
}

//get first items from contained defs
struct pairFromDef_t
{
  template<typename KeyTag, typename ValueTag, typename Def>
  constexpr auto operator()(KeyTag key_tag, ValueTag value_tag, EntitiesDef def)
  {
    return hana::make_pair(
        hana::at(*hana::findByTag(def, key_tag), 1),
        hana::at(*hana::findByTag(def, value_tag), 1)
    );
  }
}
constexpr pairFromDef_t pairFromDef{};

template<typename KeyTag, typename ValueTag, typename EntitiesDef>
constexpr auto mapify(KeyTag key_tag, ValueTag value_tag, EntitiesDef def)
{
  return hana::fold_left(
    hana::drop_front(def),
    hana::make_map(),
    hana::insert ^hana::on^ hana::partial(pairFromDef, key_tag, value_tag)
  );
}

}//nbdl_ddl_meta

#endif
