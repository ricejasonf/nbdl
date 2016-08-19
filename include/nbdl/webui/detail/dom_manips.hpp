//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_WEBUI_DETAIL_DOM_MANIPS_HPP
#define NBDL_WEBUI_DETAIL_DOM_MANIPS_HPP

#include <nbdl/webui/html.hpp>

#include <emscripten/val.h>
#include <utility>

namespace nbdl::webui::detail
{
  // TODO maybe begin/end don't belong in this file
  // begin/end tags for dispatch of begin_fn and end_fn
  struct begin { };
  struct end   { };

  struct is_begin_fn
  {
    template <typename Types>
    constexpr auto operator()(Types)
    {
      return hana::equal(hana::at_c<0>(Types{}), hana::type_c<begin>);
    }
  };
  
  constexpr is_begin_fn is_begin{};

  struct is_end_fn
  {
    template <typename Types>
    constexpr auto operator()(Types)
    {
      return hana::equal(hana::at_c<0>(Types{}), hana::type_c<end>);
    }
  };

  constexpr is_end_fn is_end{};

  template <typename ...>
  struct action_fn;

  template <typename HtmlTagName>
  struct action_fn<begin, html::tag::element_t, HtmlTagName>
  {
    template <typename ParentElement>
    emscripten::val operator()(ParentElement&& p) const
    {
      auto el = emscripten::val::global("document").template
        call<emscripten::val>("createElement", emscripten::val(hana::to<char const*>(HtmlTagName{})));
      std::forward<ParentElement>(p).template
        call<void>("appendChild", el);
      return el;
    }
  };

  template <typename HtmlTagName>
  struct action_fn<end, html::tag::element_t, HtmlTagName>
  {
    template <typename ParentElement>
    emscripten::val operator()(ParentElement&& p) const
    {
      return std::forward<ParentElement>(p)["parentNode"];
    }
  };
}
#endif
