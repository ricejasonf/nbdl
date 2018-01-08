//
// Copyright Jason Rice 2018
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_UI_HELPER_MATCH_PATH_SPEC_HPP
#define NBDL_FWD_UI_HELPER_MATCH_PATH_SPEC_HPP

#include <boost/hana/bool.hpp>

namespace nbdl::ui_helper
{
  namespace hana = boost::hana;

  //
  // match_path_spec -  Uses a PathSpec to get a value in a store
  //                    It does not call the function if it doesn't match
  //
  struct match_path_spec_fn
  {
    template <typename Store, typename PathSpec, typename Fn>
    void operator()(Store const&, PathSpec, Fn&& fn) const;
  };

  constexpr match_path_spec_fn match_path_spec{};

  template <typename T, typename PathSpec>
  struct match_path_spec_predicate_fn
  {
    template <typename Store>
    bool operator()(Store const&) const;
  };

  template <typename PathSpec>
  struct match_path_spec_predicate_fn<void, PathSpec>
  {
    template <typename Store>
    hana::true_ operator()(Store const&) const
    { return {}; }
  };

  template <typename T, typename PathSpec>
  constexpr match_path_spec_predicate_fn<T, PathSpec> match_path_spec_predicate{};
}

#endif
