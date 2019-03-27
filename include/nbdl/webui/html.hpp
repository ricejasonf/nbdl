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
    MPDEF_DIRECTIVE_LIST(set_property)
    MPDEF_DIRECTIVE_LIST(element)
    MPDEF_DIRECTIVE_LIST(event_attribute)
    MPDEF_DIRECTIVE_LIST(add_attribute_if)
    MPDEF_DIRECTIVE_LIST(add_class_if)
    MPDEF_DIRECTIVE_LIST(hidden_file_input)

    template <typename ...Args>
    constexpr auto div(Args ...args)
    { return element(hana::string<'d', 'i', 'v'>{}, args...); }

    template <typename ...Args>
    constexpr auto span(Args ...args)
    { return element(hana::string<'s', 'p', 'a', 'n'>{}, args...); }

    using pre(using auto ...args)
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
    constexpr auto form(Args ...args)
    { return element(hana::string<'f', 'o', 'r', 'm'>{}, args...); }

    using text_area(using auto ...args)
    { return element(hana::string<'t', 'e', 'x', 't', 'a', 'r', 'e', 'a'>{}, args...); }

    using h1(using auto ...args)
    { return element(hana::string<'h', '1'>{}, args...); }
    using h2(using auto ...args)
    { return element(hana::string<'h', '2'>{}, args...); }
    using h3(using auto ...args)
    { return element(hana::string<'h', '3'>{}, args...); }
    using h4(using auto ...args)
    { return element(hana::string<'h', '4'>{}, args...); }
    using h5(using auto ...args)
    { return element(hana::string<'h', '5'>{}, args...); }
    using h6(using auto ...args)
    { return element(hana::string<'h', '6'>{}, args...); }

    using attr_class(using auto ...args)
    { return attribute(hana::string<'c', 'l', 'a', 's', 's'>{}, args...); }

    using attr_href(using auto ...args)
    { return attribute(hana::string<'h', 'r', 'e', 'f'>{}, args...); }

    template <typename ...Args>
    constexpr auto on_click(Args ...args)
    { return event_attribute(hana::string<'c', 'l', 'i', 'c', 'k'>{}, args...); }

    template <typename ...Args>
    constexpr auto on_key_down(Args ...args)
    { return event_attribute(hana::string<'k', 'e', 'y', 'd', 'o', 'w', 'n'>{}, args...); }

    template <typename ...Args>
    constexpr auto on_key_up(Args ...args)
    { return event_attribute(hana::string<'k', 'e', 'y', 'u', 'p'>{}, args...); }

    namespace tag
    {
      template <typename T>
      struct add_class_when_t {};

      template <typename T>
      constexpr auto add_class_when = hana::type_c<add_class_when_t<T>>;
    }

    template <typename T>
    struct add_class_when_fn {
      static using operator()(using auto class_name, using auto path)
      {
        return mpdef::tree_node<
          decltype(tag::add_class_when<T>)
        , mpdef::list<decltype(mpdef::to_constant(class_name)), decltype(path)>
        >{};
      }
    };

    template <typename T>
    constexpr add_class_when_fn<T> add_class_when{};

    // Key used to lookup event object in store
    // for use in relevant event handlers.
    struct event_data_t { };
    constexpr event_data_t event_data{};
  }
}

#endif
