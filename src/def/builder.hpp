//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_COMMON_DEFINITION_HPP
#define NBDL_DEF_COMMON_DEFINITION_HPP

#include<boost/hana.hpp>
#include "directives.hpp"

namespace nbdl_def {
namespace builder {

  namespace hana = boost::hana;

  namespace action {
    using Create  = tag::Create_t;
    using Read    = tag::Read_t;
    using Update  = tag::Update_t;
    using Delete  = tag::Delete_t;
  }//action

  namespace channel {
    struct Upstream {};
    struct Downstream {};
  }//channel


}//builder
}//nbdl_def

#endif
