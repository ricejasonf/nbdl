//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CATCH_HPP
#define NBDL_CATCH_HPP

#include <nbdl/detail/pipe_rejection.hpp>

#include <boost/hana/type.hpp>
#include <utility>

namespace nbdl
{
  namespace hana = boost::hana;

  namespace detail
  {
    struct catch_tag { };

    template <typename RejectionHandler>
    struct catch_t
    {
      using hana_tag = catch_tag;

      RejectionHandler fn;

      template <typename ...Args>
      auto operator()(pipe_rejection, Args&& ...args)
        -> decltype(fn(std::forward<Args>(args)...))
      {
        fn(std::forward<Args>(args)...);
      }
    };
  }

  struct catch_fn
  {
    template <typename RejectionHandler>
    auto operator()(RejectionHandler&& fn) const
    {
      return detail::catch_t<RejectionHandler>{std::forward<RejectionHandler>(fn)};
    }
  };

  constexpr catch_fn catch_;
}

#endif
