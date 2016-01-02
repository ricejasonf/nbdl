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
constexpr auto createTagFilter = hana::curry<2>(hana::flip(filterByTag));

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
constexpr auto createTagFinder = hana::curry<2>(hana::flip(findByTag));

//searches a sequence of parent nodes for a tag from leaf to root
//todo fix this
struct FindSetting
{
  template<typename Xs, typename Tag>
  constexpr auto operator()(Xs&& xs, Tag&& tag)
  {
    constexpr auto find = hana::partial(hana::flip(findByTag), std::forward<Tag>(tag));
    return hana::maybe(
      hana::nothing,
      find, //ugh
      hana::find_if(xs, hana::is_just ^hana::on^ find)
    );
  }
};
constexpr FindSetting findSetting{};
constexpr auto createSettingFinder = hana::curry<2>(hana::flip(findSetting));

namespace details {

template<typename Pred>
struct AllInTreeFinder
{
  Pred const& pred;

  AllInTreeFinder(Pred const& p) :
    pred(p)
  {}

  template<typename Ancestors, typename Tree>
  constexpr auto helper(Ancestors&& ancestors, Tree&& tree, hana::true_)
  {
    return hana::make_tuple(
      hana::make_pair(std::forward<Tree>(tree), std::forward<Ancestors>(ancestors))
    );
  }

  template<typename Ancestors, typename Tree>
  constexpr auto helper(Ancestors, Tree, hana::false_)
  {
    return hana::make_tuple();
  }

  template<typename Ancestors, typename Tree>
  auto operator()(Ancestors&& ancestors, Tree&& tree)
  {
    auto cond = pred(tree);
    auto children = hana::second(tree);
    auto child_ancestors = hana::prepend(ancestors, tree);
    return hana::concat(
      helper(
        std::forward<Ancestors>(ancestors),
        std::forward<Tree>(tree),
        cond
      ),
      hana::flatten(
        hana::unpack(children,
          [&](auto&&... x) {
            return hana::make_tuple((*this)(child_ancestors, x)...);
          }
        )
      )
    );
  }
};

}//details

//returns ((node, ancestor_nodes)...)
struct FindAllInTree
{
  template<typename Tree, typename Pred>
  constexpr auto operator()(Tree&& tree, Pred const& pred)
  {
    return decltype(
      details::AllInTreeFinder<Pred>(pred)(
        hana::tuple<>{},
        std::forward<Tree>(tree)
      )
    ){};
  }
};
constexpr FindAllInTree findAllInTree{};
constexpr auto createAllInTreeFinder = hana::curry<2>(hana::flip(findAllInTree));

}//nbdl_def_meta

#endif
