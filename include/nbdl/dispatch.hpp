//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_PROCESS_MESSAGE_HPP
#define NBDL_PROCESS_MESSAGE_HPP

// #include<nbdl/concepts/Store.hpp> FIXME
#include<nbdl/fwd/dispatch.hpp>

#include<boost/hana/core/when.hpp>

namespace nbdl
{
  template<typename Store, typename Message>
  constexpr auto dispatch_fn::operator()(Store&& s, Message&& m) const
  {
    using Tag = hana::tag_of_t<Store>;
    using Impl = dispatch_impl<Tag>;

    /* FIXME: Store concept conflicts with legacy nbdl::Store
    static_assert(nbdl::Store<Tag>::value,
      "nbdl::dispatch(store, message) requires 'store' to be a Store");
    */

    return Impl::apply(std::forward<Store>(s), std::forward<Message>(m));
  };

  template<typename Tag, bool condition>
  struct dispatch_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    static constexpr auto apply(...) = delete;
  };
} // nbdl

#endif
