//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DETAIL_PIPE_REJECTION_HPP
#define NBDL_DETAIL_PIPE_REJECTION_HPP

#include <type_traits>

namespace nbdl::detail
{
  // detail::pipe_rejection is provided for use in
  // nbdl::pipe and nbdl::catch
  struct pipe_rejection {};

  template <typename ...T>
  struct is_pipe_rejection : std::false_type
  { };

  template <typename ...T>
  struct is_pipe_rejection<pipe_rejection, T...> : std::true_type
  { };
}

#endif
