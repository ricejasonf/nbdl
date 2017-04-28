//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_UI_VIEW_SPEC_HPP
#define NBDL_UI_VIEW_SPEC_HPP

#ifndef __OBJC__
static_assert(false, "UIView manipulations require an Objective-C++ compiler");
#endif

#import <UIKit/UIKit.h>

#include <mpdef/MPDEF_DIRECTIVE.hpp>

namespace nbdl::ui_view
{
  namespace hana  = boost::hana;

  namespace spec
  {
    MPDEF_DIRECTIVE_LIST(view)

    template <typename ...Args>
    constexpr auto ui_view(Args ...args)
    { return view(hana::type_c<UIView>, args...); }
  }
}

#endif
