//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_WEBUI_DETAIL_FLATTEN_SPEC_HPP
#define NBDL_WEBUI_DETAIL_FLATTEN_SPEC_HPP

#include <nbdl/fwd/webui/detail/flatten_spec.hpp>
#include <nbdl/ui_spec.hpp>
#include <nbdl/webui/detail/dom_manips.hpp>
#include <nbdl/webui/html.hpp>

#include <boost/hana/flatten.hpp>
#include <boost/hana/unpack.hpp>
#include <mpdef/list.hpp>
#include <mpdef/pair.hpp>
#include <utility>

namespace nbdl::webui::detail
{
  namespace hana  = boost::hana;

  template <typename FlattenSpecFn>
  struct flatten_named_node_fn
  {
    template <typename Tag, typename ChildNodes>
    constexpr auto operator()(Tag, ChildNodes) const
    {
      using Name = decltype(hana::at_c<0>(ChildNodes{}));
      return hana::flatten(mpdef::make_list(
        mpdef::make_list(mpdef::make_list(action_fn<begin, typename Tag::type, Name>{})),
        hana::unpack(
          hana::drop_front(ChildNodes{}), mpdef::make_list ^hana::on^ FlattenSpecFn{}
        ),
        mpdef::make_list(mpdef::make_list(action_fn<end, typename Tag::type, Name>{}))
      ));
    }
  };

#if 0 // Not Currently used.
  template <typename FlattenSpecFn>
  struct flatten_node_fn
  {
    template <typename Tag, typename ChildNodes>
    constexpr auto operator()(Tag, ChildNodes) const
    {
      return hana::flatten(mpdef::make_list(
        mpdef::make_list(mpdef::make_list(action_fn<begin, typename Tag::type>{})),
        hana::unpack(ChildNodes{}, mpdef::make_list ^hana::on^ FlattenSpecFn{}),
        mpdef::make_list(mpdef::make_list(action_fn<end, typename Tag::type>{}))
      ));
    }
  };
#endif
  template <typename FlattenSpecFn>
  struct flatten_match_node_helper_fn
  {
    template <typename T, typename Spec>
    constexpr auto operator()(mpdef::pair<T, Spec>) const
    {
      using Flattened = decltype(FlattenSpecFn{}(Spec{}));
      return mpdef::pair<hana::type<T>, Flattened>{};
    }
  };

  template <typename FlattenSpecFn>
  struct flatten_match_node_fn
  {
    template <typename Path, typename Map>
    constexpr auto operator()(ui_spec::match_t<Path, Map>) const
    {
      return mpdef::list<action_fn<
        ui_spec::match_tag
      , Path
      , decltype(hana::transform(Map{}, flatten_match_node_helper_fn<FlattenSpecFn>{}))
      >>{};
    }
  };

  template <typename Node>
  constexpr auto flatten_spec_fn::operator()(Node node) const
  {
    if constexpr(hana::Product<Node>::value)
    {
      // nodes are always pairs (tag, stuff)
      constexpr auto current_tag = hana::first(node);
      constexpr auto child_nodes = hana::second(node);

      if constexpr(decltype(hana::equal(current_tag, html::tag::element))::value)
      {
        return hana::flatten(flatten_named_node_fn<flatten_spec_fn>{}(current_tag, child_nodes));
      }
      else if constexpr(decltype(hana::equal(current_tag, html::tag::attribute))::value)
      {
        return mpdef::make_list(action_fn<
          typename decltype(current_tag)::type
        , decltype(hana::at_c<0>(child_nodes))
        , decltype(hana::at_c<1>(child_nodes))
        >{});
      }
      else if constexpr(decltype(hana::equal(current_tag, html::tag::text_node))::value)
      {
        return mpdef::make_list(action_fn<
          typename decltype(current_tag)::type
        , decltype(hana::at_c<0>(child_nodes))
        >{});
      }
    }
    else if constexpr(hana::is_a<ui_spec::match_tag, Node>())
    {
      return flatten_match_node_fn<flatten_spec_fn>{}(Node{});
    }
    else
    {
      return mpdef::make_list(node);
    }
  }
}
#endif
