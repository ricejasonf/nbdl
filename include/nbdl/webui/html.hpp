//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_HTML_HPP
#define NBDL_HTML_HPP

#include <mpdef/MPDEF_DIRECTIVE.hpp>

namespace nbdl::webui
{
  namespace hana = boost::hana;

  namespace html
  {
    MPDEF_DIRECTIVE_LEAF(text_node)
    MPDEF_DIRECTIVE_LEAF(unsafe_set_inner_html)
    MPDEF_DIRECTIVE_LIST(attribute)
    MPDEF_DIRECTIVE_LIST(element)
    MPDEF_DIRECTIVE_LIST(event_attribute)
    MPDEF_DIRECTIVE_LIST(add_class_if)

    template <typename ...Args>
    constexpr auto div(Args ...args)
    { return element(hana::string<'d', 'i', 'v'>{}, args...); }

    template <typename ...Args>
    constexpr auto pre(Args ...args)
    { return element(hana::string<'p', 'r', 'e'>{}, args...); }

    template <typename ...Args>
    constexpr auto ul(Args ...args)
    { return element(hana::string<'u', 'l'>{}, args...); }

    template <typename ...Args>
    constexpr auto li(Args ...args)
    { return element(hana::string<'l', 'i'>{}, args...); }

    template <typename ...Args>
    constexpr auto a(Args ...args)
    { return element(hana::string<'a'>{}, args...); }

    template <typename ...Args>
    constexpr auto p(Args ...args)
    { return element(hana::string<'p'>{}, args...); }

    template <typename ...Args>
    constexpr auto attr_class(Args ...args)
    { return attribute(hana::string<'c', 'l', 'a', 's', 's'>{}, args...); }

    template <typename ...Args>
    constexpr auto attr_href(Args ...args)
    { return attribute(hana::string<'h', 'r', 'e', 'f'>{}, args...); }

    template <typename ...Args>
    constexpr auto on_click(Args ...args)
    { return event_attribute(hana::string<'c', 'l', 'i', 'c', 'k'>{}, args...); }

    namespace tag
    {
      template <typename T>
      struct add_class_when_t {};

      template <typename T>
      constexpr auto add_class_when = hana::type_c<add_class_when_t<T>>;
    }

    template <typename T>
    constexpr auto add_class_when = [](auto class_name, auto path)
    {
      return mpdef::tree_node<
        decltype(tag::add_class_when<T>)
      , mpdef::list<decltype(class_name), decltype(path)>
      >{};
    };
  }
}

#endif
