//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_UI_HELPER_HPP
#define NBDL_UI_HELPER_HPP

#include <nbdl/ui_spec.hpp>

#include <boost/hana/transform.hpp>
#include <boost/hana/type.hpp>
#include <mpdef/list.hpp>
#include <mpdef/pair.hpp>
#include <utility>

namespace nbdl::ui_helper
{
  namespace hana = boost::hana;

  /*
   * flatten_match_node
   */
  template <typename FlattenSpecFn>
  struct flatten_match_node_helper_fn
  {
    template <typename T, typename Spec>
    constexpr auto operator()(mpdef::pair<T, Spec>) const
    {
      using Flattened = decltype(std::declval<FlattenSpecFn>()(Spec{}));
      return mpdef::pair<hana::type<T>, Flattened>{};
    }
  };

  struct flatten_match_node_fn
  {
    template <typename Template, typename FlattenSpecFn, typename Path, typename Map>
    constexpr auto operator()(Template tpl, FlattenSpecFn const&, ui_spec::match_t<Path, Map>) const
    {
      return mpdef::list<typename decltype(tpl(
        hana::type_c<ui_spec::match_tag>
      , hana::type_c<Path>
      , hana::type_c<
          decltype(hana::transform(Map{}, flatten_match_node_helper_fn<FlattenSpecFn>{}))
        >
      ))::type>{};
    }
  };

  constexpr flatten_match_node_fn flatten_match_node{};

  /*
   * flatten_param
   */

  struct param_action_tag { };

  template <typename ...>
  struct param_action_t { };

  struct flatten_param_node_fn
  {
    template <typename Node>
    constexpr auto operator()(Node) const
    {
      if constexpr(hana::Product<Node>::value)
      {
        constexpr auto current_tag = hana::first(Node{});
        constexpr auto child_nodes = hana::second(Node{});

        if constexpr(hana::equal(ui_spec::tag::concat, current_tag))
        {
          return hana::flatten(hana::unpack(
            child_nodes
          , hana::on(mpdef::make_list, flatten_param_node_fn{})
          ));
        }
        else
        {
          static_assert(hana::type_c<Node> == hana::type_c<void>);
        }
      }
      else if constexpr(hana::is_a<ui_spec::match_tag, Node>())
      {
        return flatten_match_node(hana::template_<param_action_t>, flatten_param_node_fn{}, Node{});
      }
      else
      {
        return mpdef::list<Node>{};
      }
    }
  };

  constexpr flatten_param_node_fn flatten_param_node{};
}

namespace boost::hana
{
  template <typename ...T>
  struct tag_of<nbdl::ui_helper::param_action_t<T...>>
  {
    using type = nbdl::ui_helper::param_action_tag;
  };
}
#endif
