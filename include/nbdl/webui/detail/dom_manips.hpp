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

  template <typename AttributeName, typename Strings>
  struct action_fn<html::tag::attribute_t, AttributeName, Strings>
  {
    // Right now Strings is just a single compile-time string
    template <typename ParentElement>
    auto operator()(ParentElement&& p) const
    {
      auto el = p.template call<emscripten::val>(
        "setAttribute"
      , emscripten::val(hana::to<char const*>(AttributeName{}))
      , emscripten::val(hana::to<char const*>(Strings{}))
      );
      return std::forward<ParentElement>(p);
    }
  };

  template <typename Strings>
  struct action_fn<html::tag::text_content_t, Strings>
  {
    // Right now Strings is just a single compile-time string
    template <typename ParentElement>
    auto operator()(ParentElement&& p) const
    {
      p.template set(
        "textContent"
      , emscripten::val(hana::to<char const*>(Strings{}))
      );
      return std::forward<ParentElement>(p);
    }
  };
}
#endif
