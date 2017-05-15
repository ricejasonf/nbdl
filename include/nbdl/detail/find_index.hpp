//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DETAIL_FIND_INDEX_HPP
#define NBDL_DETAIL_FIND_INDEX_HPP

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
      return nbdl::promise([&](auto& resolver, auto const& value, std::size_t i)
      {
        // Reject when we find a matching predicate.
        if constexpr(hana::and_(hana::Constant<decltype(pred(value))>{}, decltype(pred(value)){}))
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
            resolver.resolve(value, i + 1);
          }
        }
      });
    }
  };

  constexpr predicate_promise_fn predicate_promise{};

  struct find_index_fn
  {
    template <typename Preds, typename Value>
    std::size_t operator()(Preds const& preds, Value&& value) const
    {
      std::size_t result = 0;

      nbdl::run_sync(
        nbdl::pipe(
          hana::transform(preds, predicate_promise)
        , [](auto const& ...args)
          {
            static_assert(
              sizeof...(args) > 9000
            , "nbdl::find_index must have at least one predicate "
              "that returns compile-time Logical that is true."
            );
          }
        , nbdl::catch_([&](std::size_t index) { result = index; })
        )
      , std::forward<Value>(value)
      , std::size_t{0}
      );

      return result;
    }
  };

  constexpr find_index_fn find_index{};
}

#endif

