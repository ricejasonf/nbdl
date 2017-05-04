//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_UI_VIEW_DETAIL_FLATTEN_SPEC_HPP
#define NBDL_FWD_UI_VIEW_DETAIL_FLATTEN_SPEC_HPP

namespace nbdl::ui_view::detail
{
  struct flatten_spec_fn
  {
    template <typename Node>
    constexpr auto operator()(Node node) const;
  };

  constexpr flatten_spec_fn flatten_spec{};
}
#endif
