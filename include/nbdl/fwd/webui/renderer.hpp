//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_WEBUI_RENDERER_HPP
#define NBDL_WEBUI_RENDERER_HPP

namespace nbdl { namespace webui
{
  // tag still needs RenderSpec
  template <typename RenderSpec>
  struct renderer;

  template <typename PushApi, typename RenderSpec>
  class renderer_impl;
}}
#endif
