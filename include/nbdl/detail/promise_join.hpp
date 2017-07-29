//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DETAIL_PROMISE_JOIN_HPP
#define NBDL_DETAIL_PROMISE_JOIN_HPP

#include <nbdl/detail/wrap_promise.hpp>
#include <nbdl/fwd/detail/promise_join.hpp>
#include <nbdl/fwd/hold_lazy.hpp>

#include <boost/hana/concept/foldable.hpp>
#include <boost/hana/core/is_a.hpp>
#include <boost/hana/fold_right.hpp>
#include <boost/hana/integral_constant.hpp>
#include <boost/hana/type.hpp>
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
    else if constexpr(decltype(hana::is_a<make_lazy_holder_tag, Current>)::value)
    {
      using Promise = decltype(current(hana::typeid_(next)));

      return promise_join_t<Promise, std::decay_t<Next>>(
        std::forward<Current>(current)(hana::typeid_(next))
      , std::forward<Next>(next)
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

#endif
