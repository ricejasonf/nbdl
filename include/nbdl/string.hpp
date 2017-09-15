//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_BIND_STRING_HPP
#define NBDL_BIND_STRING_HPP

#include <nbdl/concept/String.hpp>

#include <string>

namespace nbdl
{
  struct string : std::string
  {
    using std::string::string;
  };

  template <>
  struct String<string> : std::true_type
  { };
}

#endif
