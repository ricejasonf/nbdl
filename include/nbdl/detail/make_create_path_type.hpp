//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DETAIL_MAKE_CREATE_PATH_TYPE_HPP
#define NBDL_DETAIL_MAKE_CREATE_PATH_TYPE_HPP

#include <nbdl/message.hpp>

#include <boost/hana/back.hpp>
#include <boost/hana/drop_back.hpp>
#include <type_traits>

namespace nbdl::detail
{
  namespace hana = boost::hana;

  template <typename Path>
  using make_create_path_key_t = std::decay_t<decltype(hana::back(std::declval<Path>()))>;

  template <typename Path>
  using enable_if_path_key_not_empty_t = std::enable_if_t<
    not std::is_empty<make_create_path_key_t<Path>>::value
  >;

  template <typename Path
          , typename Channel = message::channel::upstream
          , typename Action = message::action::create
          , typename = void
          >
  struct make_create_path
  {
    using type = Path;
  };
  
  template <typename Path>
  struct make_create_path<Path
                        , message::channel::upstream
                        , message::action::create
                        , enable_if_path_key_not_empty_t<Path>
                        >
  {
    using Key = make_create_path_key_t<Path>;
    using ParentPath = decltype(hana::drop_back(std::declval<Path>(), hana::size_c<1>));

    using type = message::create_path<Key, ParentPath>;
  };

  template <typename Path>
  using make_create_path_t = typename make_create_path<Path>::type;
}

#endif
