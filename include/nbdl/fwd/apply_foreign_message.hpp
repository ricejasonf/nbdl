//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_APPLY_FOREIGN_MESSAGE_HPP
#define NBDL_FWD_APPLY_FOREIGN_MESSAGE_HPP

#include<boost/hana/core/when.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  template<typename T, typename = void>
  struct apply_foreign_message_impl : apply_foreign_message_impl<T, hana::when<true>> { };

  struct apply_foreign_message_fn
  {
    /*
     * Store
     * Message
     * Fn - fn(path)
     *  Callback to notify when state changes at a given path.
     */
    template<typename Store, typename Message, typename Fn>
    constexpr void operator()(Store&&, Message&&, Fn&&) const;
  };

  constexpr apply_foreign_message_fn apply_foreign_message{};
} // nbdl

#endif
