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
#include <boost/mp11/list.hpp>
#include <mpdef/list.hpp>
#include <mpdef/pair.hpp>
#include <type_traits>
#include <utility>

namespace nbdl::webui::detail
{
  namespace hana = boost::hana;
  using namespace boost::mp11;

  template <typename T>
  constexpr auto is_add_class_when_tag = hana::false_c;

  template <typename T>
  constexpr auto is_add_class_when_tag<html::tag::add_class_when_t<T>> = hana::true_c;

  template <typename FlattenSpecFn>
  struct flatten_named_node_fn
  {
    template <typename Tag, typename ChildNodes>
    constexpr auto operator()(Tag, ChildNodes) const
    {
      using Name  = decltype(hana::at_c<0>(ChildNodes{}));
      using Nodes = decltype(hana::drop_front(ChildNodes{}));
      constexpr auto children = hana::unpack(Nodes{}, [](auto ...x)
      {
        return mp_append<decltype(FlattenSpecFn{}(x))...>{};
      });

      return mp_append<
        mpdef::list<action_fn<begin, typename Tag::type, Name>>
      , std::decay_t<decltype(children)>
      , mpdef::list<action_fn<end, typename Tag::type, Name>>
      >{};
    }
  };

  template <typename Child>
  constexpr auto flatten_text_node(Child child)
  {
    if constexpr(hana::Product<Child>::value)
    {
      if constexpr(hana::equal(ui_spec::tag::concat, hana::typeid_(hana::first(child))))
      {
        return hana::unpack(hana::second(child), [](auto ...xs)
        {
          return mp_append<decltype(flatten_text_node(xs))...>{};
        });
      }
      else
      {
        // invalid argument to text node
        return mpdef::list<
          action_fn<html::tag::text_node_t, void>
        >{};
      }
    }
    else
    {
      return mpdef::list<
        action_fn<html::tag::text_node_t , decltype(child)>
      >{};
    }
  }

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
        return decltype(flatten_named_node_fn<flatten_spec_fn>{}(current_tag, child_nodes)){};
      }
      else if constexpr(
            decltype(hana::equal(current_tag, html::tag::attribute))::value
         or decltype(hana::equal(current_tag, html::tag::set_property))::value
      )
      {
        return mpdef::list<action_fn<
          typename decltype(current_tag)::type
        , decltype(hana::at_c<0>(child_nodes))
        , decltype(ui_helper::flatten_param_node(hana::at_c<1>(child_nodes)))
        >>{};
      }
      else if constexpr(decltype(hana::equal(current_tag, html::tag::text_node))::value)
      {
        return flatten_text_node(child_nodes);
      }
      else if constexpr(decltype(hana::equal(current_tag, html::tag::unsafe_set_inner_html))::value)
      {
        return mpdef::list<action_fn<
          typename decltype(current_tag)::type
        , std::decay_t<decltype(child_nodes)>
        >>{};
      }
      else if constexpr(decltype(hana::or_(
        hana::equal(current_tag, html::tag::add_class_if)
      , hana::equal(current_tag, html::tag::event_attribute)
      , hana::equal(current_tag, html::tag::hidden_file_input)
      , is_add_class_when_tag<typename decltype(current_tag)::type>
      ))::value)
      {
        return mpdef::list<mp_append<
          action_fn<typename decltype(current_tag)::type>
        , std::decay_t<decltype(child_nodes)>
        >>{};
      }
      else
      {
        static_assert(std::is_void<decltype(current_tag)>::value);
      }
    }
    else if constexpr(hana::is_a<ui_spec::branch_spec_tag, Node>())
    {
      return ui_helper::flatten_branch_node(
        hana::template_<action_fn>
      , flatten_spec_fn{}
      , Node{}
      );
    }
    else if constexpr(hana::is_a<ui_spec::for_each_tag, Node>())
    {
      return ui_helper::flatten_for_each_node(
        hana::template_<action_fn>
      , flatten_spec_fn{}
      , Node{}
      );
    }
    else if constexpr(hana::is_a<ui_spec::noop_t, Node>())
    {
      return mpdef::list<>{};
    }
    else
    {
      return mpdef::list<Node>{};
    }
  }
}
#endif
