//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DETAIL_RESOLVE_PATH_HPP
#define NBDL_DETAIL_RESOLVE_PATH_HPP

namespace nbdl::detail
{
  template <typename PathSpec, typename S>
  void resolve_path(S&& store)
  {
    // TODO get a flattened tuple of every path spec in PathSpec
    // depth first
    // match with each resolved path and if one does not match its
    // match_type type then do a noop
    nbdl::match(
      std::forward<S>(store),
      
    );
  }
}

#endif
