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
  template <typename Tag, typename Name = void>
  struct begin_fn { };

  template <typename Tag, typename Name = void>
  constexpr begin_fn<Tag, Name> begin{};

  template <typename Tag, typename Name = void>
  struct end_fn { };

  template <typename Tag, typename Name = void>
  constexpr end_fn<Tag, Name> end{};

  template <typename HtmlTagName>
  struct begin_fn<html::tag::element_t, HtmlTagName>
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
  struct end_fn<html::tag::element_t, HtmlTagName>
  {
    template <typename ParentElement>
    emscripten::val operator()(ParentElement&& p) const
    {
      return std::forward<ParentElement>(p)["parentNode"];
    }
  };
}
#endif
