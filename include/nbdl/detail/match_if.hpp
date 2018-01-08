//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DETAIL_MATCH_IF_HPP
#define NBDL_DETAIL_MATCH_IF_HPP

#include <mpdef/list.hpp>
#include <nbdl/catch.hpp>
#include <nbdl/pipe.hpp>
#include <nbdl/promise.hpp>
#include <nbdl/run_sync.hpp>

#include <boost/hana/and.hpp>
#include <boost/hana/bool.hpp>
#include <boost/hana/concept/constant.hpp>
#include <boost/hana/transform.hpp>
#include <boost/mp11/algorithm.hpp>
#include <utility>

namespace nbdl::detail
{
  namespace hana = boost::hana;
  using namespace boost::mp11;

  template <typename Pred>
  struct predicate_promise
  {
    using hana_tag = nbdl::promise_tag;

    template <typename Resolver, typename Value>
    void resolve(Resolver& resolver, Value const& value, std::size_t i) const
    {
      using Result = std::decay_t<decltype(Pred{}(value))>;

      // Reject when we find a matching predicate.
      if constexpr(hana::Constant<Result>::value)
      {
        if constexpr(Result::value)
          resolver.reject(i);
        else
          resolver.resolve(value, i + 1);
      }
      else
      {
        if (Pred{}(value))
        {
          resolver.reject(i);
        }
        else
        {
          resolver.resolve(value, i + 1);
        }
      }
    }

    template <typename Resolver, typename ...Args>
    void reject(Resolver&& resolver, Args&&... args) noexcept
    {
      // propagate rejection
      std::forward<Resolver>(resolver).reject(std::forward<Args>(args)...);
    }
  };

  struct match_if_promise_fn
  {
    template <typename ...Preds>
    auto operator()(mpdef::list<Preds...>) const
    {
      return nbdl::promise([](auto& resolve, auto&& value)
      {
        std::size_t index;

        nbdl::run_sync(
          hana::make_tuple(
            predicate_promise<Preds>{}...
          , [](auto const& ...args)
            {
              static_assert(
                sizeof...(args) > 9000
              , "nbdl::detail::match_if must have at least one predicate "
                "that returns compile-time Logical that is true."
              );
            }
          , nbdl::catch_([&](std::size_t result) { index = result; })
          )
        , std::forward<decltype(value)>(value)
        , hana::size_c<0>
        );

        mp_with_index<sizeof...(Preds)>(index, resolve);
      });
    }
  };

  constexpr match_if_promise_fn match_if_promise{};

  struct match_if_fn
  {
    template <typename ...Preds, typename Value>
    int operator()(mpdef::list<Preds...>, Value&& value) const
    {
      int index;

      nbdl::run_sync(
        hana::make_tuple(
          predicate_promise<Preds>{}...
        , [](auto const& ...args)
          {
            static_assert(
              sizeof...(args) > 9000
            , "nbdl::detail::match_if must have at least one predicate "
              "that returns compile-time Logical that is true."
            );
          }
        , nbdl::catch_([&](std::size_t result) { index = result; })
        )
      , std::forward<Value>(value)
      , hana::size_c<0>
      );

      return index;
    }
  };

  constexpr match_if_fn match_if{};
}

#endif
