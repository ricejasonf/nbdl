//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DETAIL_PROMISE_JOIN_HPP
#define NBDL_DETAIL_PROMISE_JOIN_HPP

#include <nbdl/fwd/detail/promise_join.hpp>

#include <nbdl/concept/Resolver.hpp>
#include <nbdl/detail/wrap_promise.hpp>

#include <boost/hana/concept/foldable.hpp>
#include <boost/hana/fold_right.hpp>
#include <boost/hana/functional/id.hpp>
#include <boost/hana/integral_constant.hpp>
#include <utility>

namespace nbdl::detail
{
  template <typename Current, typename Next>
  auto promise_join_fn::operator()(Current&& current, Next&& next) const
  {
    if constexpr(hana::Foldable<Current>::value)
    {
      return hana::fold_right(
        std::forward<Current>(current)
      , std::forward<Next>(next)
      , detail::promise_join_fn{}
      );
    }
    else
    {
      using WrappedPromise = decltype(wrap_promise(std::forward<Current>(current)));

      return promise_join_t<WrappedPromise, std::decay_t<Next>>(
        wrap_promise(std::forward<Current>(current))
      , std::forward<Next>(next)
      );
    }
  }
}

namespace nbdl
{
  namespace hana = boost::hana;

  template <>
  struct Resolver<detail::promise_join_tag>
    : hana::true_
  { };

  template <>
  struct Resolver<detail::promise_join_nested_tag>
    : hana::true_
  { };
}

#endif
