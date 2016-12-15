//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_PROMISE_HPP
#define NBDL_PROMISE_HPP

#include <boost/hana/core/is_a.hpp>
#include <utility>

namespace nbdl
{
  namespace hana = boost::hana;

  namespace detail
  {
    struct promise_tag { };

    template <typename Fn>
    struct promise_t
    {
      using hana_tag = promise_tag;

      Fn fn;

      template <typename ...Args>
      void operator()(Args&& ...args)
      {
        fn(std::forward<Args>(args)...);
      }
    };
  }

  struct promise_fn
  {
    template <typename Fn>
    auto operator()(Fn&& fn) const
    {
      if constexpr(hana::is_a<detail::promise_tag, std::decay_t<Fn>>())
      {
        // idempotent
        return std::forward<Fn>(fn);
      }
      else
      {
        return detail::promise_t<Fn>{std::forward<Fn>(fn)};
      }
    }
  };

  constexpr promise_fn promise;
}

#endif
