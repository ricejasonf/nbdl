//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_WEBUI_DETAIL_FLATTEN_SPEC_HPP
#define NBDL_WEBUI_DETAIL_FLATTEN_SPEC_HPP

#include <nbdl/fwd/webui/detail/flatten_spec.hpp>
#include <nbdl/ui_helper.hpp>
#include <nbdl/ui_spec.hpp>
#include <nbdl/webui/detail/dom_manips.hpp>
#include <nbdl/webui/html.hpp>

#include <boost/hana/flatten.hpp>
#include <boost/hana/type.hpp>
#include <boost/hana/unpack.hpp>
#include <kvasir/mpl/sequence/join.hpp>
#include <mpdef/list.hpp>
#include <mpdef/pair.hpp>
#include <type_traits>
#include <utility>

namespace nbdl::webui::detail
{
  namespace hana  = boost::hana;
  namespace kmpl = kvasir::mpl;

  template <typename FlattenSpecFn>
  struct flatten_named_node_fn
  {
    template <typename Tag, typename ChildNodes>
    constexpr auto operator()(Tag, ChildNodes) const
    {
      using Name = decltype(hana::at_c<0>(ChildNodes{}));

      return kmpl::join<
        kmpl::list<action_fn<begin, typename Tag::type, Name>>
      , mpdef::unpack_to_flatten_kmpl_list<decltype(hana::drop_front(ChildNodes{})), FlattenSpecFn>
      , kmpl::list<action_fn<end, typename Tag::type, Name>>
      >{};
    }
  };

  struct flatten_spec_impl_fn
  {
    template <typename Node>
    constexpr auto operator()(Node node) const
    {
      if constexpr(hana::Product<Node>::value)
      {
        // nodes are always pairs (tag, stuff)
        constexpr auto current_tag = hana::first(node);
        constexpr auto child_nodes = hana::second(node);

        if constexpr(decltype(hana::equal(current_tag, html::tag::element))::value)
        {
          return decltype(flatten_named_node_fn<flatten_spec_impl_fn>{}(current_tag, child_nodes)){};
        }
        else if constexpr(decltype(hana::equal(current_tag, html::tag::attribute))::value)
        {
          return kmpl::list<action_fn<
            typename decltype(current_tag)::type
          , decltype(hana::at_c<0>(child_nodes))
          , mpdef::to_mpdef_list<decltype(ui_helper::flatten_param_node(hana::at_c<1>(child_nodes)))>
          >>{};
        }
        else if constexpr(decltype(hana::or_(
          hana::equal(current_tag, html::tag::text_node)
        , hana::equal(current_tag, html::tag::unsafe_set_inner_html)
        ))::value)
        {
          static_assert(
            hana::not_(hana::is_a<char const*>(child_nodes))
          , "Raw string literals cannot be used at compile-time"
          );
          return kmpl::list<action_fn<
            typename decltype(current_tag)::type
          , std::decay_t<decltype(child_nodes)>
          >>{};
        }
      }
      else if constexpr(   hana::is_a<ui_spec::match_tag, Node>()
                        || hana::is_a<ui_spec::match_if_tag, Node>()
                       )
      {
        return ui_helper::flatten_match_node(
          hana::template_<action_fn>
        , flatten_spec_impl_fn{}
        , Node{}
        );
      }
      else
      {
        return kmpl::list<Node>{};
      }
    }
  };

  template <typename Node>
  constexpr auto flatten_spec_fn::operator()(Node node) const
  {
    return mpdef::to_mpdef_list<decltype(flatten_spec_impl_fn{}(node))>{};
  }
}
#endif
