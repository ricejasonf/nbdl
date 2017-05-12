//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_PARAMS_PROMISE_HPP
#define NBDL_PARAMS_PROMISE_HPP

#include <nbdl/catch.hpp>
#include <nbdl/path_promise.hpp>
#include <nbdl/promise.hpp>
#include <nbdl/ui_helper.hpp>
#include <nbdl/ui_spec.hpp>

#include <boost/hana/core/is_a.hpp>
#include <boost/hana/functional/arg.hpp>
#include <boost/hana/functional/id.hpp>
#include <functional>
#include <utility>

namespace nbdl
{
  namespace hana = boost::hana;

  struct params_promise_fn
  {
    template <typename ...Params>
    auto operator()(mpdef::list<Params...>) const;
  };

  constexpr params_promise_fn params_promise{};

  namespace detail
  {
    struct make_param_promise_fn
    {
      auto operator()() const
      {
        return hana::id;
      }

      template <typename Param>
      auto operator()(Param) const;
    };

    constexpr make_param_promise_fn make_param_promise{};

    struct make_param_promise_match_fn
    {
      template <typename T>
      struct match_branch_pred_fn
      {
        template <typename U, typename Spec>
        constexpr auto operator()(mpdef::pair<U, Spec>) const
        {
          return hana::or_(
            hana::type_c<T>    == U{}
          , hana::type_c<void> == U{}
          );
        }
      };

      template <typename PathSpec, typename Branches>
      auto operator()(ui_helper::param_action_t<ui_spec::match_tag, PathSpec, Branches>) const
      {
        return nbdl::promise([](auto& resolve, auto const& store, auto const& tuple)
        {
          nbdl::run_sync(nbdl::pipe(
            nbdl::path_promise(PathSpec{})
          , [&](auto const& value)
            {
              using T = typename decltype(hana::typeid_(value))::type;
              constexpr auto result = hana::find_if(Branches{}, match_branch_pred_fn<T>{});
              static_assert(
                hana::is_just(result)
              , "`ui_spec::match` has no matching branch"
              );

              nbdl::run_sync(nbdl::pipe(
                params_promise(hana::second(*result))
              , [&](auto const& result)
                {
                  resolve(store, hana::concat(tuple, result));
                }
              ), store);
            }
          ), store);
        });
      }
    };

    template <typename Param>
    auto make_param_promise_fn::operator()(Param) const
    {
      if constexpr(hana::is_a<ui_spec::path_tag, Param>)
      {
        return nbdl::promise([](auto& resolve, auto const& store, auto const& tuple)
        {
          nbdl::run_sync(
            nbdl::pipe(
              nbdl::path_promise(Param{})
            , [&](auto const& result) { resolve(store, hana::append(tuple, std::cref(result))); }
            , nbdl::catch_(nbdl::noop)
            )
          , store
          );
        });
      }
      else if constexpr(hana::is_a<ui_helper::param_action_tag, Param>)
      {
        return make_param_promise_match_fn{}(Param{});
      }
      else
      {
        return nbdl::promise([](auto& resolve, auto const& store, auto const& tuple)
        {
          resolve(store, hana::append(tuple, Param{}));
        });
      }
    }
  }

  template <typename ...Params>
  auto params_promise_fn::operator()(mpdef::list<Params...>) const
  {
    return nbdl::pipe(
      nbdl::promise([](auto& resolve, auto const& store) { resolve(store, hana::make_tuple()); })
    , detail::make_param_promise(Params{})...
    , hana::arg<2> // (store, tuple) -- we just want the tuple
    );
  }
}

#endif