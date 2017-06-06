//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_ENUMERATE_ACCESS_POINTS_HPP
#define NBDL_DEF_BUILDER_ENUMERATE_ACCESS_POINTS_HPP

#include <mpdef/append_if.hpp>
#include <mpdef/collect_settings.hpp>
#include <mpdef/compose_collectors.hpp>
#include <mpdef/find_in_tree.hpp>
#include <mpdef/list.hpp>
#include <mpdef/tree_node.hpp>
#include <nbdl/def/builder/access_point_meta.hpp>
#include <nbdl/def/builder/access_point_path_key.hpp>
#include <nbdl/def/directives.hpp>
#include <nbdl/null_store.hpp>

#include <boost/hana.hpp>
#include <utility>

namespace nbdl_def {
namespace builder {

namespace hana = boost::hana;

namespace enum_access_points_detail {
  // TODO Refactor point-free code here. It is hard to read
  // and compiles slow.
  struct is_access_point_def_fn
  {
    template <typename X>
    constexpr auto operator()(X x) const
    {
      // nested ifs for lazy evaluation
      if constexpr(mpdef::is_tree_node(x))
      {
        if constexpr(hana::equal(tag::AccessPoint, hana::first(x)))
        {
          return hana::true_c;
        }
        else
        {
          return hana::false_c;;
        }
      }
      else
      {
        return hana::false_c;
      }
    }
  };

  template <typename TransformFn>
  struct collect_list_fn
  {
    template <typename State, typename X>
    constexpr auto operator()(State state, X x) const
    {
      // nested ifs for lazy evaluation
      if constexpr(mpdef::is_tree_node(x))
      {
        if constexpr(hana::equal(tag::AccessPoint, hana::first(x)))
        {
          return hana::append(state, TransformFn{}(hana::second(x)));
        }
        else
        {
          return state;
        }
      }
      else
      {
        return state;
      }
    }
  };

  constexpr auto collect_entities = collect_list_fn<decltype(
    hana::reverse_partial(hana::at_key, tag::Entity)
  )>{};
  constexpr auto collect_path_nodes = collect_list_fn<access_point_path_key_fn>{};

  //constexpr auto settings = mpdef::with_settings(tag::Store);
  constexpr auto collector = mpdef::compose_collectors(
    //mpdef::collect_settings,
    //collect_entities,
    collect_path_nodes
  );
  constexpr auto matcher = mpdef::create_in_tree_finder(is_access_point_def_fn{}, collector);
  constexpr auto initial_summary = mpdef::make_list(
    mpdef::make_list()
  );

  struct has_actions_fn
  {
    // T is a product of (node, summary)
    template <typename T>
    constexpr auto operator()(T t) const
    {
      return hana::or_(
        hana::greater(hana::maybe(
          hana::size_c<0>,
          hana::length,
          hana::find(hana::second(hana::first(t)), tag::Actions)
        ), hana::size_c<0>),
        hana::not_(hana::equal(
          hana::type_c<nbdl::null_store>,
          hana::find(hana::second(hana::first(t)), tag::Store).value_or(hana::type_c<nbdl::null_store>)
        ))
      );
    }
  };

  struct helper_helper_fn
  {
    template<typename Helper, typename HelperResult>
    constexpr auto operator()(Helper const& helper, HelperResult&& result)
    {
      return hana::unpack(hana::second(hana::first(result)),
        mpdef::make_list ^hana::on^ hana::partial(helper, hana::second(result)));
    }
  };

  constexpr helper_helper_fn helperhelper{};

  struct helper_fn
  {
    template<typename Summary, typename Def>
    constexpr auto operator()(Summary&& summary, Def&& def) const
    {
      using hana::on;

      auto results = matcher(std::forward<Summary>(summary), std::forward<Def>(def));
      return hana::concat(
        hana::filter(results, has_actions_fn{}),
        hana::flatten(hana::flatten(hana::unpack(std::move(results),
          mpdef::make_list ^on^ hana::partial(helperhelper, *this)
        )))
      );
    }
  };
  constexpr helper_fn helper{};

  struct result_helper_fn
  {
    template<typename Result>
    constexpr auto operator()(Result&& result)
    {
      auto node_children = hana::second(hana::first(result));
      auto summary       = hana::second(result);
      auto entity        = node_children[tag::Entity];
      auto path_nodes    = hana::at(summary, hana::int_c<0>);
      return builder::make_access_point_meta(
        node_children[tag::Name],
        hana::find(node_children, tag::Actions).value_or(mpdef::make_list()),
        hana::find(node_children, tag::Store).value_or(hana::type_c<nbdl::null_store>),
        mpdef::make_list(entity),
        hana::typeid_(hana::unpack(path_nodes, hana::template_<hana::tuple>))
      );
    }
  };
  constexpr result_helper_fn result_helper{};

}//enum_access_points_detail

/*
 * Gets AccessPoints that have actions. Returns list of AccessPointMeta
 */
struct enumerate_access_points_fn
{
  template<typename Def>
  constexpr auto operator()(Def&& def) const
  {
    using enum_access_points_detail::helper;
    using enum_access_points_detail::result_helper;
    using enum_access_points_detail::initial_summary;
    using hana::on;

    return hana::unpack(helper(initial_summary, std::forward<Def>(def)),
      mpdef::make_list ^on^ result_helper);
  }
};
constexpr enumerate_access_points_fn enumerate_access_points{};

}//builder
}//nbdl_def
#endif
