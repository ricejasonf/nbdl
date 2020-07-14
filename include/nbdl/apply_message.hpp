//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_APPLY_MESSAGE_HPP
#define NBDL_APPLY_MESSAGE_HPP

#include <nbdl/apply_foreign_message.hpp>
#include <nbdl/concept/NetworkStore.hpp>
#include <nbdl/fwd/apply_message.hpp>
#include <nbdl/message.hpp>

#include <boost/hana/core/default.hpp>
#include <boost/hana/core/tag_of.hpp>
#include <boost/hana/integral_constant.hpp>
#include <utility>

namespace nbdl
{
  namespace hana = boost::hana;

  template <NetworkStore Store, typename Message>
  constexpr auto apply_message_fn::operator()(Store&& s, Message&& m) const {
    using Tag = hana::tag_of_t<Store>;
    using Impl = apply_message_impl<Tag>;

#if 0 // TODO add Message concept
    static_assert(nbdl::Message<Message>::value,
      "nbdl::apply_message(store, message) requires 'message' to be a Message");
#endif

#if 0 // TODO This would make a useful early error message
    static_assert(
      !hana::is_convertible<
        decltype(Impl::apply(std::forward<Store>(s), std::forward<Message>(m)))
      , bool
      >::value
    , "nbdl::apply_message must return a value convertible to bool"
    );
#endif
    return Impl::apply(std::forward<Store>(s), std::forward<Message>(m));
  };

  template<typename Tag>
  struct apply_message_impl : hana::default_ {
    template <typename Store, typename Message>
    static constexpr auto apply(Store&& s, Message&& m) {
      bool has_state_changed = false;
      nbdl::apply_foreign_message(
        std::forward<Store>(s)
      , std::forward<Message>(m)
      , [&](auto const&) { has_state_changed = true; }
      );
      return has_state_changed;
    }
  };
}

#endif
