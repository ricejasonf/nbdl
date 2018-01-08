//
// Copyright Jason Rice 2018
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_UI_HELPER_MATCH_PARAMS_SPEC_HPP
#define NBDL_FWD_UI_HELPER_MATCH_PARAMS_SPEC_HPP

namespace nbdl::ui_helper
{
  struct match_params_spec_fn
  {
    template <typename Store, typename ParamsSpec, typename Fn>
    void operator()(Store, ParamsSpec, Fn&&) const;
  };

  constexpr match_params_spec_fn match_params_spec{};
}

#endif
