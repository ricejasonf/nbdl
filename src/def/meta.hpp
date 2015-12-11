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

struct FilterByTag
{
  template<typename Xs, typename Tag>
  constexpr auto operator()(Xs&& xs, Tag&& tag) const
  {
    return hana::transform(
      hana::filter(std::forward<Xs>(xs), tagMatch(std::forward<Tag>(tag))),
      hana::second
    );
  }
};
constexpr FilterByTag filterByTag{};
constexpr auto createTagFilter = hana::curry(hana::flip(filterByTag));

struct FindByTag
{
  template<typename Xs, typename Tag>
  constexpr auto operator()(Xs&& xs, Tag&& tag) const
  {
    return hana::maybe(hana::nothing, hana::just ^hana::on^ hana::second, 
      hana::find_if(std::forward<Xs>(xs), tagMatch(std::forward<Tag>(tag))));
  }
};
constexpr FindByTag findByTag{};
constexpr auto createTagFinder = hana::curry(hana::flip(findByTag));

//searches a sequence of parent nodes for a tag from leaf to root
struct FindSetting
{
  template<typename Xs, typename Tag>
  constexpr auto operator()(Xs&& xs, Tag&& tag)
  {
    constexpr auto find = hana::partial(hana::flip(findByTag), std::forward<Tag>(tag));
    return find(hana::find_if(xs, hana::is_just ^hana::on^ find));
  }
};
constexpr FindSetting findSetting{};
constexpr auto createSettingFinder = hana::curry(hana::flip(findSetting));

struct FindAllInTree
{
  template<typename Tree, typename Pred>
  constexpr auto operator()(Tree&& tree, Pred&& pred)
  {
    //todo finish this 
  }
};

//not used?
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



//not used??
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
