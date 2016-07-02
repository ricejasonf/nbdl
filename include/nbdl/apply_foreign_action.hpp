//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_APPLY_FOREIGN_ACTION_HPP
#define NBDL_APPLY_FOREIGN_ACTION_HPP

#include<nbdl/fwd/apply_foreign_action.hpp>

#include<nbdl/concept/Store.hpp>

#include<utility>

namespace nbdl
{
  template<typename Store, typename Message, typename Fn>
  constexpr void apply_foreign_action_fn::operator()(Store&& s, Message&& m, Fn&& fn) const
  {
    using Tag = hana::tag_of_t<Store>;
    using Impl = apply_foreign_action_impl<Tag>;

    static_assert(nbdl::Store<Store>::value,
      "nbdl::apply_foreign_action(store, message, fn) requires 'store' to be a Store");

    return Impl::apply(std::forward<Store>(s), std::forward<Message>(m), std::forward<Fn>(fn));
  };

  template<typename Tag, bool condition>
  struct apply_foreign_action_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    template <typename ...Args>
    static constexpr void apply(Args&& ...)
    {
      // do nothing
    }
  };
} // nbdl

#endif
