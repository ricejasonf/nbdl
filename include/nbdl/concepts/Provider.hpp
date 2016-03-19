//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPTS_PROVIDER_HPP
#define NBDL_CONCEPTS_PROVIDER_HPP

#include<nbdl/fwd/make_provider.hpp>
#include<nbdl/fwd/send_upstream_message.hpp>

#include<boost/hana/core/default.hpp>
#include<boost/hana/core/tag_of.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  template<typename T>
  struct Provider
  {
    using Tag = typename hana::tag_of<T>::type;
    static constexpr bool value =
          !hana::is_default<nbdl::make_provider_impl          <Tag>>::value
      &&  !hana::is_default<nbdl::send_upstream_message_impl  <Tag>>::value
      ;
  };
} // nbdl

#endif
