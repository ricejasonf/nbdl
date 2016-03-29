//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MAKE_PROVIDER_HPP
#define NBDL_MAKE_PROVIDER_HPP

#include<nbdl/concept/Provider.hpp>
#include<nbdl/fwd/make_provider.hpp>

namespace nbdl
{
  template <typename T>
  template <typename PushFn, typename ...Args>
  constexpr decltype(auto) make_provider_fn<T>::operator()(PushFn&& push, Args&& ...args) const
  {
    using Tag = hana::tag_of_t<T>;
    using Impl = make_provider_impl<Tag>;
    using Return = decltype(Impl::apply(std::forward<PushFn>(push), std::forward<Args>(args)...));

    static_assert(
      nbdl::Provider<Return>::value
      , "nbdl::make_provider<T>(push, args...) must return a Provider.");

    return Impl::apply(std::forward<PushFn>(push), std::forward<Args>(args)...);
  };

  template <typename Tag, bool condition>
  struct make_provider_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    static constexpr auto apply(...) = delete;
  };
} // nbdl

#endif

