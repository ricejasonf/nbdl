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
      using std::string::operator=;

      string(std::string const& s)
        : std::string(s)
      { }

      string(std::string&& s)
        : std::string(std::move(s))
      { }
  };

  template <>
  inline constexpr bool is_string<string> = true;
  static_assert(String<string>);
  static_assert(String<string const&>);
}

#endif
