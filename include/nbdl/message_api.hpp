//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MESSAGE_API_HPP
#define NBDL_MESSAGE_API_HPP

#include <nbdl/def/builder/context.hpp>
#include <nbdl/entity.hpp>
#include <nbdl/variant.hpp>

#include <boost/mp11/list.hpp>
#include <string>

namespace nbdl
{
  namespace hana = boost::hana;
  namespace mp11 = boost::mp11;

  struct system_message {
    enum {
      ERROR, WARNING, STATUS, DEBUG, DEV_DEBUG
    } level;
    std::string value;
  };

  NBDL_ENTITY(system_message, level, value);

  template <typename Tag, typename SystemMessage = system_message>
  struct message_api
  {
    using upstream_variant =
      mp11::mp_apply<nbdl::variant,
      mp11::mp_push_front<
        typename nbdl_def::builder::make_context_meta_t<Tag>::message_api_meta::upstream_types
      , SystemMessage
      >>;

    using downstream_variant =
      mp11::mp_apply<nbdl::variant,
      mp11::mp_push_front<
        typename nbdl_def::builder::make_context_meta_t<Tag>::message_api_meta::downstream_types
      , SystemMessage
      >>;
  };
}

#endif
