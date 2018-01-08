//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_UI_HELPER_HPP
#define NBDL_UI_HELPER_HPP

#include <nbdl/detail/default_constructible_lambda.hpp>
#include <nbdl/ui_spec.hpp>

#include <boost/hana/transform.hpp>
#include <boost/hana/type.hpp>
#include <boost/mp11/list.hpp>
#include <mpdef/list.hpp>
#include <mpdef/pair.hpp>
#include <utility>

namespace nbdl::ui_helper
{
  namespace hana = boost::hana;
  using namespace boost::mp11;

  /*
   * flatten_branch_node
   */
  template <typename FlattenSpecFn>
  struct flatten_branch_node_helper_fn
  {
    template <typename T, typename Spec>
    constexpr auto operator()(mpdef::pair<T, Spec>) const
    {
      using Flattened = decltype(std::declval<FlattenSpecFn>()(Spec{}));
      return mpdef::pair<T, Flattened>{};
    }
  };

  struct flatten_branch_node_fn
  {
    template <typename Template, typename FlattenSpecFn, typename Tag, typename PathSpec, typename Map>
    constexpr auto operator()(Template tpl
                            , FlattenSpecFn const&
                            , ui_spec::branch_spec<Tag, PathSpec, Map>) const
    {
      using FnList = decltype(hana::transform(Map{}, flatten_branch_node_helper_fn<FlattenSpecFn>{}));
      return mpdef::list<typename decltype(tpl(
        hana::type_c<ui_spec::branch_spec<Tag, PathSpec, FnList>>
      ))::type>{};
    }
  };

  constexpr flatten_branch_node_fn flatten_branch_node{};

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
          return hana::unpack(child_nodes, [](auto ...x)
          {
            return mp_append<decltype(flatten_param_node_fn{}(x))...>{};
          });
        }
        else
        {
          static_assert(hana::type_c<Node> == hana::type_c<void>);
        }
      }
      else if constexpr(hana::is_a<ui_spec::branch_spec_tag, Node>())
      {
        return flatten_branch_node(hana::template_<param_action_t>, flatten_param_node_fn{}, Node{});
      }
      else if constexpr(hana::is_a<ui_spec::apply_tag, Node>())
      {
        using FlatParamsSpec = mp_apply<
          mp_append
        , decltype(hana::transform(mp_second<Node>{}, flatten_param_node_fn{}))
        >;
        return mpdef::list<param_action_t<ui_spec::apply_t<mp_first<Node>, FlatParamsSpec>>>{};
      }
      else if constexpr(hana::is_a<ui_spec::key_at_tag, Node>())
      {
        // just unwrap the PathSpec
        return mpdef::list<mp_first<Node>>{};
      }
      else
      {
        return mpdef::list<Node>{};
      }
    }
  };

  constexpr flatten_param_node_fn flatten_param_node{};

  struct flatten_for_each_node_fn
  {
    template <typename Template, typename FlattenSpecFn, typename Path, typename SpecFn>
    constexpr auto operator()(Template
                            , FlattenSpecFn const&
                            , ui_spec::for_each_t<Path, SpecFn>) const
    {
      using ItrKey = hana::type<SpecFn>;
      using ItrPath = decltype(ui_spec::get(ItrKey{}));
      using Spec = decltype(nbdl::detail::default_constructible_lambda<SpecFn>{}(ItrPath{}));

      return mpdef::list<typename Template::template apply<
        ui_spec::for_each_tag
      , Path
      , ItrKey
      , decltype(std::declval<FlattenSpecFn>()(Spec{}))
      >::type>{};
    }
  };

  constexpr flatten_for_each_node_fn flatten_for_each_node{};
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
