//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_UI_VIEW_RENDERER_HPP
#define NBDL_UI_VIEW_RENDERER_HPP

namespace nbdl::ui_view
{
  // tag still needs RenderSpec
  template <typename RenderSpec>
  struct renderer;

  template <typename PushApi, typename RenderSpec>
  class renderer_impl;
}
#endif
