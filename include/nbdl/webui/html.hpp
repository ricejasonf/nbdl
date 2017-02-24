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
  namespace hana  = boost::hana;

  namespace html
  {
    MPDEF_DIRECTIVE_LIST(element)
    MPDEF_DIRECTIVE_LIST(attribute)
    MPDEF_DIRECTIVE_LIST(text_content)

    template <typename ...Args>
    constexpr auto div(Args ...args)
    { return element(hana::string<'d', 'i', 'v'>{}, args...); }

    template <typename ...Args>
    constexpr auto attr_class(Args ...args)
    { return attribute(hana::string<'c', 'l', 'a', 's', 's'>{}, args...); }

    // control stuff
    //MPDEF_DIRECTIVE_LIST(template_)
    //MPDEF_DIRECTIVE_PAIR(param) // name, path
    //MPDEF_DIRECTIVE_LIST(export_)
    //MPDEF_DIRECTIVE_LEAF(match)
    //MPDEF_DIRECTIVE(match_case)
    //MPDEF_DIRECTIVE_LIST(switch_)
    //MPDEF_DIRECTIVE(case_)
    //MPDEF_DIRECTIVE_LIST(for_each)
    //MPDEF_DIRECTIVE_LIST(if_)
    //MPDEF_DIRECTIVE_LEAF(equal_to)
    //MPDEF_DIRECTIVE_LEAF(type)
  }
}

#endif
