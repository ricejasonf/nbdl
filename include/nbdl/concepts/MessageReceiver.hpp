//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPTS_MESSAGE_RECEIVER_HPP
#define NBDL_CONCEPTS_MESSAGE_RECEIVER_HPP

#include<boost/hana/core/when.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  template<typename T, typename = hana::when<true>>
  struct MessageReceiver
  {
    static constexpr bool value = false;
  };

  template<typename T>
  struct MessageReceiver<T,
    hana::when<std::is_member_function_pointer<decltype(&T::receive_message)>::value>
  >
  {
    static constexpr bool value = true;
  };

} // nbdl

#endif
