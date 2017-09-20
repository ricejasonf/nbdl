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
#include <mpdef/pair.hpp>
#include <mpdef/tree_node.hpp>
#include <nbdl/def/builder/access_point_meta.hpp>
#include <nbdl/def/builder/access_point_path_key.hpp>
#include <nbdl/def/directives.hpp>
#include <nbdl/null_store.hpp>

#include <boost/hana.hpp>
#include <utility>

namespace nbdl_def::builder
{
  namespace hana = boost::hana;

  namespace enum_access_points_detail
  {
    template <typename T>
    constexpr bool has_actions(T t)
    {
      // It has Actions or a non-null store
      return std::decay_t<decltype(hana::maybe(
               hana::size_c<0>,
               hana::length,
               hana::find(t, tag::Actions)
             ))>::value > 0
          || !decltype(hana::equal(
               hana::type_c<nbdl::null_store>,
               hana::find(t, tag::Store).value_or(hana::type_c<nbdl::null_store>)
             ))::value
          ;
    };

    struct transform_listen_paths_fn
    {
      // TreeNodes is pair<Path_t, types<...>>
      template <typename ...TreeNodes>
      constexpr auto operator()(mpdef::list<TreeNodes...>) const
      {
        return mpdef::list<
          decltype(hana::unpack(hana::second(TreeNodes{}), hana::template_<hana::tuple>))
          ...
        >{};
      }
    };

    template <typename ...PathNode, typename NodeChildren>
    inline constexpr auto make_access_point_meta_helper(mpdef::list<PathNode...>
                                                      , NodeChildren node_children)
    {
      constexpr auto actions = hana::find(node_children, tag::Actions).value_or(mpdef::make_list());
      constexpr auto listen_paths = hana::find(node_children, tag::ListenPaths).value_or(mpdef::list<>{});

      if constexpr(has_actions(node_children))
      {
        return mpdef::make_list(builder::make_access_point_meta(
          hana::find(node_children, tag::Name).value_or(hana::type_c<void>)
        , actions
        , hana::find(node_children, tag::Store).value_or(hana::type_c<nbdl::null_store>)
        , mpdef::make_list(node_children[tag::Entity])
        , hana::type_c<hana::tuple<typename PathNode::type...>>
        , transform_listen_paths_fn{}(listen_paths)
        ));
      }
      else
      {
        return mpdef::list<>{};
      }
    }

    struct helper_fn
    {
      template<typename ...PathNode, typename Def>
      constexpr auto operator()(mpdef::list<PathNode...> path_nodes, Def def) const
      {
        /*
         * NOTE:  This is depth-first and the order should not be
         *        changed as it will break interfaces created with it.
         */
        constexpr auto key = hana::first(def);
        constexpr auto node_children = hana::second(def);

        if constexpr(decltype(key == tag::AccessPoint)::value)
        {
          using NewPath = mpdef::list<PathNode..., decltype(node_children[tag::PathKey])>;
          return hana::flatten(mpdef::make_list(
            make_access_point_meta_helper(NewPath{}, node_children)
          , hana::flatten(hana::unpack(node_children, [](auto ...childs)
            { return mpdef::make_list(helper_fn{}(NewPath{} , childs)...); }))
          ));
        }
        else if constexpr(decltype(key == tag::AccessPoints)::value)
        {
          return hana::unpack(node_children, [](auto ...xs)
          { return hana::flatten(mpdef::make_list(helper_fn{}(mpdef::list<PathNode...>{}, xs)...)); });
        }
        else if constexpr(hana::is_a<mpdef::map_tag, decltype(node_children)>)
        {
          if constexpr(decltype(hana::contains(node_children, tag::AccessPoints))::value)
          {
            return helper_fn{}(path_nodes,
              mpdef::make_pair(tag::AccessPoints, hana::at_key(def, tag::AccessPoints)));
          }
          else if constexpr(decltype(hana::contains(node_children, tag::AccessPoint))::value)
          {
            return helper_fn{}(path_nodes,
              mpdef::make_pair(tag::AccessPoint, hana::at_key(def, tag::AccessPoint)));
          }
          else { return mpdef::list<>{}; }
        }
        else { return mpdef::list<>{}; }
      }
    };

    constexpr helper_fn helper{};
  }

  /*
   * Gets AccessPoints that have actions. Returns list of AccessPointMeta
   */
  struct enumerate_access_points_fn
  {
    template<typename Def>
    constexpr auto operator()(Def def) const
    {
      using enum_access_points_detail::helper;
      return helper(mpdef::list<>{}, def);
    }
  };

  constexpr enumerate_access_points_fn enumerate_access_points{};
}

#endif
