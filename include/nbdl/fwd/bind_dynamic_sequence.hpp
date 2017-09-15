//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_BIND_DYNAMIC_SEQUENCE_HPP
#define NBDL_FWD_BIND_DYNAMIC_SEQUENCE_HPP

#include <boost/hana/core/when.hpp>
#include <boost/mp11/utility.hpp>

namespace nbdl
{
  namespace hana = boost::hana;
  namespace mp11 = boost::mp11;

  template<typename T, typename = void>
  struct bind_dynamic_sequence_impl : bind_dynamic_sequence_impl<T, hana::when<true>> { };

  struct bind_dynamic_sequence_fn
  {
    template<typename BindableDynamicSequence, typename BindFn>
    constexpr auto operator()(BindableDynamicSequence&&, BindFn&&) const;
  };

  constexpr bind_dynamic_sequence_fn bind_dynamic_sequence{};

  // bind_dynamic_sequence_value_type
  // Specialize this to specify the value type for a given container
  template <typename T, typename = void>
  struct bind_dynamic_sequence_value_type;
}

#endif
