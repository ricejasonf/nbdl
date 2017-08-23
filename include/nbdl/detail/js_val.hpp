//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DETAIL_JS_VAL_HPP
#define NBDL_DETAIL_JS_VAL_HPP

#include<emscripten.h>

namespace nbdl::detail
{
  // Simple way to associate an object to a value in javascript
  // It is not the mose efficient as it simply uses a hash table
  struct js_val
  {
    static void init_registry()
    {
      EM_ASM({
        if (!Module.nbdl_detail_json_val)
        {
          window.NBDL_DETAIL_JS_GET = function(handle) {
            return Module.nbdl_detail_json_val[handle];
          };
          window.NBDL_DETAIL_JS_SET = function(handle, value) {
            Module.nbdl_detail_json_val[handle] = value;
            return Module.nbdl_detail_json_val[handle];
          };
          Module.nbdl_detail_json_val = {};
        }
      });
    }

    js_val() = default;
    js_val(js_val const&) = delete;

    ~js_val()
    {
      EM_ASM_({
        delete Module.nbdl_detail_json_val[$0];
      }, handle());
    }

    int handle() const
    {
      static_assert(sizeof(int) == sizeof(void*));
      return reinterpret_cast<int>(this);
    }
  };
}

#endif
