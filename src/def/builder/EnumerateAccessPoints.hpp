//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_ENUMERATE_ACCESS_POINTS_HPP
#define NBDL_DEF_BUILDER_ENUMERATE_ACCESS_POINTS_HPP

#include<def/builder.hpp>
#include<def/builder/AccessPointMeta.hpp>
#include<Listener.hpp>
#include<mpdef/AppendIf.hpp>
#include<mpdef/CollectSettings.hpp>
#include<mpdef/ComposeCollectors.hpp>
#include<mpdef/FindInTree.hpp>
#include<mpdef/TreeNode.hpp>
#include<Store.hpp>

#include<utility>

namespace nbdl_def {
namespace builder {

namespace enum_access_points_detail {

constexpr auto settings = mpdef::withSettings(tag::StoreContainer, tag::StoreEmitter);
constexpr auto pred = hana::demux(hana::eval_if)
(
  mpdef::is_tree_node,
  hana::make_lazy(hana::compose(hana::equal.to(tag::AccessPoint), hana::first)),
  hana::always(hana::make_lazy(hana::false_c))
);
constexpr auto collector = mpdef::composeCollectors(
  mpdef::collectSettings,
  hana::demux(hana::eval_if)
  (
    hana::demux(pred)(hana::arg<2>),
    hana::make_lazy(
      hana::demux(hana::append)
      (
        hana::arg<1>,
        hana::demux(hana::reverse_partial(hana::at_key, tag::EntityName))
        (
          hana::demux(hana::second)(hana::arg<2>)
        )
      )
    ),
    hana::make_lazy(hana::arg<1>)
  )
);
constexpr auto matcher = mpdef::createInTreeFinder(pred, collector);
constexpr auto initial_summary = hana::make_tuple(settings, hana::make_tuple());

// called on result of FindInTree which is (node, summary)
constexpr auto hasActions = hana::compose(hana::demux(hana::maybe)
(
  hana::always(hana::false_c),
  hana::always(hana::compose(hana::reverse_partial(hana::greater, hana::size_c<0>), hana::length)),
  hana::compose(hana::reverse_partial(hana::find, tag::Actions), hana::second)
), hana::first);

struct HelperHelper
{
  template<typename Helper, typename HelperResult>
  constexpr auto operator()(Helper const& helper, HelperResult&& result)
  {
    return hana::unpack(hana::second(hana::first(result)),
      hana::make_tuple ^hana::on^ hana::partial(helper, hana::second(result)));
  }
};
constexpr HelperHelper helperhelper{};

struct Helper
{
  template<typename Summary, typename Def>
  constexpr auto operator()(Summary&& summary, Def&& def) const
  {
    using hana::on;

    auto results = matcher(std::forward<Summary>(summary), std::forward<Def>(def));
    return hana::concat(
      hana::filter(results, hasActions),
      hana::flatten(hana::flatten(hana::unpack(std::move(results),
        hana::make_tuple ^on^ hana::partial(helperhelper, *this)
      )))
    );
  }
};
constexpr Helper helper{};

struct ResultHelper
{
  template<typename Result>
  constexpr auto operator()(Result&& result)
  {
    auto node_children = hana::second(hana::first(result));
    auto summary = hana::second(result);
    auto settings = summary[hana::int_c<0>];
    auto entity_names = summary[hana::int_c<1>];
    return builder::makeAccessPointMeta(
      node_children[tag::Name],
      node_children[tag::Actions],
      settings[tag::StoreContainer].value_or(
        hana::template_<nbdl::store::HashMap>
      ),
      settings[tag::StoreEmitter].value_or(
        hana::partial(hana::template_<nbdl::store_emitter::HashMap>,
          hana::type_c<nbdl::ListenerHandlerDummy<>>)
      ),
      entity_names
    );
  }
};
constexpr ResultHelper resultHelper{};

}//enum_access_points_detail

/*
 * Gets AccessPoints that have actions. Returns tuple of AccessPointMeta
 */
struct EnumerateAccessPoints
{
  template<typename Def>
  constexpr auto operator()(Def&& def) const
  {
    using enum_access_points_detail::helper;
    using enum_access_points_detail::resultHelper;
    using enum_access_points_detail::initial_summary;
    using hana::on;

    return hana::unpack(helper(initial_summary, std::forward<Def>(def)),
      hana::make_tuple ^on^ resultHelper);
  }
};
constexpr EnumerateAccessPoints enumerateAccessPoints{};

}//builder
}//nbdl_def
#endif
