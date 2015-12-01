//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_META_DEFINITION_HPP
#define NBDL_DEF_META_DEFINITION_HPP

#include<boost/hana.hpp>

#define NBDL_DEF_DIRECTIVE(NAME)                        \
namespace tag {                                         \
  struct NAME##_t {};                                   \
  constexpr auto NAME = boost::hana::type_c<NAME##_t>;  \
}                                                       \
template<typename... Tn> constexpr auto NAME(Tn... tn)  \
{ return boost::hana::make_pair(tag::NAME, boost::hana::make_tuple(tn...)); }

namespace nbdl_def_meta {

namespace hana = boost::hana;

template<typename T>
constexpr auto tagMatch(T t)
{
  return hana::equal.to(t) ^hana::on^ hana::first;
}

template<typename Xs, typename Tag>
constexpr auto filterByTag(Xs xs, Tag tag)
{
  return hana::transform(
    hana::filter(xs, tagMatch(tag)),
    hana::second
  );
}

template<typename Xs, typename Tag>
constexpr auto findByTag(Xs xs, Tag tag)
{
  return hana::maybe(hana::nothing, hana::just ^hana::on^ hana::second, 
    hana::find_if(xs, tagMatch(tag)));
}

//get first items from contained defs
struct pairFromDef_t
{
  template<typename KeyTag, typename ValueTag, typename Def>
  constexpr auto operator()(KeyTag key_tag, ValueTag value_tag, Def def)
  {
    return hana::make_pair(
        hana::at(hana::second(*findByTag(def, key_tag)), 0),
        hana::at(hana::second(*findByTag(def, value_tag)), 0)
    );
  }
};
constexpr pairFromDef_t pairFromDef{};

template<typename KeyTag, typename ValueTag, typename Def>
constexpr auto mapify(KeyTag key_tag, ValueTag value_tag, Def def)
{
  return hana::fold_left(
    hana::second(def),
    hana::make_map(),
    hana::insert ^hana::on^ hana::partial(pairFromDef, key_tag, value_tag)
  );
}

}//nbdl_def_meta

#endif
