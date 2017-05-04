//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_PARAMS_PROMISE_HPP
#define NBDL_PARAMS_PROMISE_HPP

#include <nbdl/path_promise.hpp>
#include <nbdl/promise.hpp>

#include <boost/hana/core/is_a.hpp>
#include <boost/hana/functional/arg.hpp>
#include <functional>
#include <utility>

namespace nbdl
{
  namespace hana = boost::hana;

  namespace detail
  {
    struct make_param_promise_fn
    {
      template <typename Param>
      auto operator()(Param) const
      {
        if constexpr(hana::is_a<ui_spec::path_tag, Param>)
        {
          return nbdl::promise([](auto& resolve, auto const& store, auto const& tuple)
          {
            nbdl::run_sync(
              nbdl::pipe(
                nbdl::path_promise(Param{})
              , [&](auto const& result) { resolve(store, hana::append(tuple, std::cref(result))); }
              )
            , store
            );
          });
        }
        else
        {
          return nbdl::promise([](auto& resolve, auto const& store, auto const& tuple)
          {
            resolve(store, hana::append(tuple, Param{}));
          });
        }
      }
    };
  }

  struct params_promise_fn
  {
    template <typename ...Params>
    auto operator()(mpdef::list<Params...>) const
    {
      return nbdl::pipe(
        nbdl::promise([](auto& resolve, auto const& store) { resolve(store, hana::make_tuple()); })
      , detail::make_param_promise_fn{}(Params{})...
      , hana::arg<2> // (store, tuple) -- we just want the tuple
      );
    }
  };

  constexpr params_promise_fn params_promise{};
}

#endif
