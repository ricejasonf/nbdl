//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MAKE_CONSUMER_HPP
#define NBDL_MAKE_CONSUMER_HPP

#include<nbdl/concepts/Consumer.hpp>
#include<nbdl/fwd/make_consumer.hpp>

namespace nbdl
{
  template <typename T>
  template <typename PushFn, typename ...Args>
  constexpr decltype(auto) make_consumer_fn<T>::operator()(PushFn&& push, Args&& ...args) const
  {
    using Tag = hana::tag_of_t<T>;
    using Impl = make_consumer_impl<Tag>;
    using Return = decltype(Impl::apply(std::forward<PushFn>(push), std::forward<Args>(args)...));

    static_assert(
      nbdl::Consumer<Return>::value
      "nbdl::make_consumer<T>(push, args...) must return a Consumer.");

    return Impl::apply(std::forward<PushFn>(push), std::forward<Args>(args)...);
  };

  template <typename Tag, bool condition>
  struct make_consumer_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    static constexpr auto apply(...) = delete;
  };
} // nbdl

#endif

