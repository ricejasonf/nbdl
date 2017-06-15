//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DETAIL_MAKE_CREATE_PATH_TYPE_HPP
#define NBDL_DETAIL_MAKE_CREATE_PATH_TYPE_HPP

#include <nbdl/message.hpp>

#include <boost/hana.hpp>

namespace nbdl::detail
{
  namespace hana = boost::hana;

  // The representation of paths is implementation defined.
  // (
  //   Currently it's just a tuple where a "create path"
  //   has the last node wrapped as a hana::type.
  // )
  template <typename Path
          , typename Channel = message::channel::upstream
          , typename Action = message::action::create>
  struct make_create_path
  {
    using type = Path;
  };
  
  template <typename Path>
  struct make_create_path<Path, message::channel::upstream, message::action::create>
  {
    using KeyType = decltype(hana::typeid_(hana::back(std::declval<Path>())));

    using type = decltype(hana::append(
      hana::drop_back(std::declval<Path>(), hana::size_c<1>),
      KeyType{}
    ));
  };

  constexpr auto make_create_path_type = hana::metafunction<make_create_path>;
}

#endif
