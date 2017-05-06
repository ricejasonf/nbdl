//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_WEBUI_RENDERER_HPP
#define NBDL_FWD_WEBUI_RENDERER_HPP

namespace nbdl::webui
{
  template <typename RenderSpec>
  struct renderer { };

  template <typename Store, typename RenderSpec, typename IsSpecFlat>
  struct renderer_impl;
}
#endif
