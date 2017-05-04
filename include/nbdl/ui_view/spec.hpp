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

#define NBDL_OBJC_SELECTOR(NAME) struct NAME##_t { \
  SEL operator()() const { return @selector(NAME); } }; \
  constexpr NAME##_t NAME{};

namespace nbdl::ui_view
{
  namespace hana  = boost::hana;

  namespace spec
  {
    MPDEF_DIRECTIVE_LIST(view)

    namespace selector
    {
      NBDL_OBJC_SELECTOR(setAttributedText);
      NBDL_OBJC_SELECTOR(setCenter);
      NBDL_OBJC_SELECTOR(sizeToFit);
    }

    template <typename ...Args>
    constexpr auto ui_view(Args ...args)
    { return view(hana::type_c<UIView>, args...); }

    template <int x, int y, int w, int h>
    struct init_with_frame_fn
    {
      template <typename View>
      auto operator()(View* view) const
      {
        [view initWithFrame: CGRectMake(x, y, w, h)];
        [view setBackgroundColor: [UIColor clearColor]];
      }
    };

    template <int x, int y, int w, int h>
    constexpr init_with_frame_fn<x, y, w, h> init_with_frame{};

  }
}

#endif
