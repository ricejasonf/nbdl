//
// Copyright Jason Rice 2019
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DETAIL_COOKIE_PARSER_HPP
#define NBDL_DETAIL_COOKIE_PARSER_HPP

#include <cwctype>
#include <string_view>

namespace nbdl::app::detail {
  std::string_view remove_leading_whitespace(std::string_view x) {
    while (x.size() > 0 && std::iswspace(x[0])) {
      x.remove_prefix(1);
    }
    return x;
  }

  std::string_view skip_past(std::string_view x, char c) {
    auto pos = x.find(c);
    if (pos == x.npos || x.size() == 0) {
      // the expected token is not present
      return {};
    }
    x.remove_prefix(pos + 1);
    return x;
  }

  std::string_view  trim_double_quotes(std::string_view x) {
    if (x.size() > 1 && x.find('"') == 0) {
      // assumes the end is also a double quote
      x.remove_prefix(1);
      x.remove_suffix(1);
    }
    return x;
  }

  // key, value, remaining string after `;`
  std::string_view get_session_cookie(std::string_view x) {
    while (x.size() > 0) {
      x = remove_leading_whitespace(x);
      auto pos = x.find("NBDL_SESS=");
      if (pos == 0) {
        // get the value after "NBDL_SESS="
        x = skip_past(x, '=');
        auto trim_pos = x.find(';');
        if (trim_pos != x.npos) {
          x.remove_suffix(x.size() - trim_pos);
        }
        return x;
      }

      x = skip_past(x, '=');
      x = skip_past(x, ';');
    }

    return x;
  }
}

#endif
