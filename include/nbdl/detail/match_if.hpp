//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DETAIL_MATCH_IF_HPP
#define NBDL_DETAIL_MATCH_IF_HPP

#include <nbdl/run_sync.hpp>
#include <nbdl/pipe.hpp>
#include <nbdl/promise.hpp>
#include <nbdl/catch.hpp>

#include <boost/hana/and.hpp>
#include <boost/hana/bool.hpp>
#include <boost/hana/concept/constant.hpp>
#include <boost/hana/transform.hpp>
#include <utility>

namespace nbdl::detail
{
  namespace hana = boost::hana;

  struct predicate_promise_fn
  {
    template <typename Pred>
    auto operator()(Pred const& pred) const
    {
      return nbdl::promise([&](auto& resolver, auto const& value, auto i)
      {
        // Reject when we find a matching predicate.
        if constexpr(hana::and_(
          hana::Constant<decltype(pred(value))>{}
        , decltype(pred(value)){})
        )
        {
          resolver.reject(i);
        }
        else
        {
          if (pred(value))
          {
            resolver.reject(i);
          }
          else
          {
            resolver.resolve(value, i + hana::size_c<1>);
          }
        }
      });
    }
  };

  constexpr predicate_promise_fn predicate_promise{};

  struct match_if_fn
  {
    template <typename Preds>
    auto operator()(Preds const& preds) const
    {
      return nbdl::promise([&](auto& resolve, auto&& value)
      {
        nbdl::run_sync(
          nbdl::pipe(
            hana::transform(hana::to_tuple(preds), predicate_promise)
          , [](auto const& ...args)
            {
              static_assert(
                sizeof...(args) > 9000
              , "nbdl::detail::match_if must have at least one predicate "
                "that returns compile-time Logical that is true."
              );
            }
          , nbdl::catch_([&](auto index) { resolve(index); })
          )
        , std::forward<decltype(value)>(value)
        , hana::size_c<0>
        );
      });
    }
  };

  constexpr match_if_fn match_if{};
}

#endif
