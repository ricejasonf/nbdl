//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_STORE_HPP
#define NBDL_DEF_BUILDER_STORE_HPP

#include<def/builder.hpp>
#include<Listener.hpp>
#include<Store.hpp>

namespace nbdl_def {
namespace builder {

struct Store
{
  template<typename PathType, typename AccessPoint>
  constexpr auto operator()(PathType path_type, AccessPoint access_point) const
  {
    auto store_emitter = access_point.storeEmitter()(path_type);
    //this adds a requirement for StoreEmitter to export a ListenerHandler
    auto listener_handler = hana::type_c<typename decltype(store_emitter)::type::ListenerHandler>;
    return hana::template_<nbdl::Store>(
        access_point.storeContainer()(path_type),
        store_emitter,
        listener_handler,
        path_type
      );
  }
};
constexpr Store store{};

}//builder
}//nbdl_def
#endif
