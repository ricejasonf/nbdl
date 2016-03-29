//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_APPLY_ACTION_HPP
#define NBDL_APPLY_ACTION_HPP

#include<nbdl/fwd/apply_action.hpp>

#include<nbdl/concept/Store.hpp>

#include<utility>

namespace nbdl
{
  template<typename Store, typename Message>
  constexpr bool apply_action_fn::operator()(Store&& s, Message&& m) const
  {
    using Tag = hana::tag_of_t<Store>;
    using Impl = apply_action_impl<Tag>;

    static_assert(nbdl::Store<Store>::value,
      "nbdl::apply_action(store, message) requires 'store' to be a Store");

    return Impl::apply(std::forward<Store>(s), std::forward<Message>(m));
  };

  template<typename Tag, bool condition>
  struct apply_action_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    static constexpr bool apply(...) = delete;

      // typical use case
      //
      // for non-read actions the dispatcher will probably want
      // to propagate the state change to consumers via a new
      // message or a change notification except for weird edge
      // cases like write-only or null stores
      //
      // maybe make action Read use a uid (it already does)
      // to ensure the requestor is the only one receiving
      // the response to prevent unnecessary traffic
      // there would have to be a special store just for read requests
      //
      // when a client app is waiting on a read it should queue changes
      // to that resource and squash the whole thing before telling the
      // consumer. this should probably be handled internally
  };
} // nbdl

#endif
