//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_HOLD_LAZY_HPP
#define NBDL_HOLD_LAZY_HPP

#include <nbdl/fwd/hold_lazy.hpp>
#include <nbdl/promise.hpp>

#include <cassert>
#include <optional>
#include <memory>
#include <utility>
#include <type_traits>

namespace nbdl
{
  namespace detail
  {
    template <typename F>
    struct lazy_holder_function
    {
      F f;
    };

    template <typename F, typename T>
    struct lazy_holder
      : lazy_holder_function<F>
    {
      lazy_holder(F f)
        : lazy_holder_function<F>{std::move(f)}
        , null_state()
        , engaged(false)
      { }

      // The move constructor default initializes
      // the value because it is assumed the instance
      // is settled after the promise is initialized
      // (ie it is never moved once `value` is set)
      lazy_holder(lazy_holder&& old)
        : lazy_holder_function<F>{std::move(old.f)}
        , null_state()
        , engaged(false)
      {
        assert(!old.engaged);
      }

      ~lazy_holder()
      {
        if (engaged)
        {
          value.~T();
        }
      }

      template <typename Resolver, typename ...Xs>
      void operator()(Resolver& resolver, Xs&& ...xs) noexcept
      {
        new(std::addressof(value)) T(this->f(resolver, std::forward<Xs>(xs)...));
        engaged = true;
      }

    private:

      union {
        char null_state;
        T value;
      };
      bool engaged;
    };

    template <typename F, typename Args>
    struct make_lazy_holder_fn;

    template <typename F, typename ...Args>
    struct make_lazy_holder_fn<F, lazy_args_t<Args...>>
      : lazy_holder_function<F>
    {
      using hana_tag = make_lazy_holder_tag;

      make_lazy_holder_fn(F f_)
        : lazy_holder_function<F>{std::move(f_)}
      { }

      template <typename ResolverType>
      auto operator()(ResolverType) //&&
      {
        using Resolver = typename ResolverType::type;
        using T = std::invoke_result_t<F, Resolver&, Args...>;
        return nbdl::promise(lazy_holder<F, T>{std::move(this->f)});
      }
    };
  }

  template <typename F>
  auto hold_lazy_fn::operator()(F&& f) const
  {
    return detail::make_lazy_holder_fn<std::decay_t<F>, lazy_args_t<>>{
      std::forward<F>(f)
    };
  }

  template <typename F, typename ...Args>
  auto hold_lazy_fn::operator()(F&& f, lazy_args_t<Args...>) const
  {
    return detail::make_lazy_holder_fn<std::decay_t<F>, lazy_args_t<Args...>>{
      std::forward<F>(f)
    };
  }
}

#endif
