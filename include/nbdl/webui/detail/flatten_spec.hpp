//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_WEBUI_DETAIL_FLATTEN_SPEC_HPP
#define NBDL_WEBUI_DETAIL_FLATTEN_SPEC_HPP

#include <nbdl/webui/detail/dom_manips.hpp>
#include <nbdl/webui/html.hpp>

#include <boost/hana/experimental/types.hpp>
#include <boost/hana/flatten.hpp>
#include <boost/hana/functional/compose.hpp>
#include <boost/hana/unpack.hpp>
#include <utility>

namespace nbdl::webui::detail
{
  namespace hana  = boost::hana;
  namespace hanax = boost::hana::experimental;

  struct flatten_spec_fn;

  template <typename FlattenSpecFn>
  struct flatten_named_node_fn
  {
    template <typename Tag, typename ChildNodes>
    constexpr auto operator()(Tag, ChildNodes) const
    {
      using Name = decltype(hana::at_c<0>(ChildNodes{}));
      return hana::flatten(mpdef::make_list(
        mpdef::make_list(mpdef::make_list(hanax::types<begin, typename Tag::type, Name>{})),
        hana::unpack(
          hana::drop_front(ChildNodes{}), mpdef::make_list ^hana::on^ FlattenSpecFn{}
        ),
        mpdef::make_list(mpdef::make_list(hanax::types<end, typename Tag::type, Name>{}))
      ));
    }
  };

  template <typename FlattenSpecFn>
  struct flatten_node_fn
  {
    template <typename Tag, typename ChildNodes>
    constexpr auto operator()(Tag, ChildNodes) const
    {
      return hana::flatten(mpdef::make_list(
        mpdef::make_list(mpdef::make_list(hanax::types<begin, typename Tag::type>{})),
        hana::unpack(ChildNodes{}, mpdef::make_list ^hana::on^ FlattenSpecFn{}),
        mpdef::make_list(mpdef::make_list(hanax::types<end, typename Tag::type>{}))
      ));
    }
  };

  struct flatten_spec_fn
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
          return hana::flatten(flatten_named_node_fn<flatten_spec_fn>{}(current_tag, child_nodes));
        }
        else if constexpr(decltype(hana::equal(current_tag, html::tag::attribute))::value)
        {
          return hana::flatten(flatten_named_node_fn<flatten_spec_fn>{}(current_tag, child_nodes));
        }
        // else if some control thingy
      }
      else
      {
        return mpdef::make_list(node);
      }
    }
  };

  constexpr flatten_spec_fn flatten_spec{};
}
#endif
