//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CATCH_HPP
#define NBDL_CATCH_HPP

#include <nbdl/concept/Resolver.hpp>

#include <boost/hana/functional/overload_linearly.hpp>
#include <boost/hana/integral_constant.hpp>
#include <boost/hana/type.hpp>
#include <utility>

namespace nbdl
{
  namespace hana = boost::hana;

  struct catch_tag { };

  template <typename RejectionHandler>
  struct catch_t
  {
    using hana_tag = catch_tag;

    RejectionHandler fn;

    template <typename Resolver, typename ...Args>
    void resolve(Resolver&& r, Args&&... args) noexcept
    {
      // just propagate resolved value
      std::forward<Resolver>(r).resolve(std::forward<Args>(args)...);
    };

    template <typename Resolver, typename ...Args>
    void reject(Resolver& r, Args&&... args) noexcept
    {
      // uses libc++ impl details FIXME
      if constexpr(std::__invokable<RejectionHandler, Args...>::value)
      {
        fn(std::forward<Args>(args)...);
        r.terminate();
      }
      else
      {
        r.reject(std::forward<Args>(args)...);
      }
#if 0
      // try fn or propagate rejection
      hana::overload_linearly(
        fn // doesn't call terminate :(
      , [&](auto&& ...xs)
        {
          r.reject(std::forward<decltype(xs)>(xs)...);
        }
      )(std::forward<Args>(args)...);
#endif
    }
  };

  struct catch_fn
  {
    template <typename RejectionHandler>
    auto operator()(RejectionHandler&& fn) const
    {
      return catch_t<RejectionHandler>{std::forward<RejectionHandler>(fn)};
    }
  };

  constexpr catch_fn catch_;

  template <>
  struct Resolver<catch_tag>
    : hana::true_
  { };
}

#endif
