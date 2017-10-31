//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_WEBSOCKET_API_DETAIL_CONVERT_PAYLOAD_HPP
#define NBDL_WEBSOCKET_API_DETAIL_CONVERT_PAYLOAD_HPP

#include <nbdl/detail/js_val.hpp>
#include <nbdl/websocket_api/json_payload.hpp>

#include <emscripten.h>
#include <vector>

namespace nbdl::websocket_api::detail
{
  // In place convserions of a javascript
  // string or ArrayBuffer to an endpoint payload value.

  template <typename Payload>
  struct convert_payload;

  template <typename Char, typename Allocator>
  struct convert_payload<std::vector<Char, Allocator>>
  {
    static_assert(sizeof(Char) == 1);

    static void apply(nbdl::detail::js_val const& val, std::vector<unsigned char>& p)
    {
      int len = EM_ASM_INT(
        {
          return Module.NBDL_DETAIL_JS_GET($0).length;
        }
      , val.handle()
      );
      p.resize(len);
      p.clear();
      EM_ASM_(
        {
          var v = Module.NBDL_DETAIL_JS_GET($0);
          if (typeof v === 'string')
          {
            Module.stringToAscii(v, $1);
          }
          else // assume its an ArrayBuffer
          {
            var heap = new Uint8Array(Module.HEAPU8.buffer, $1, v.length);
            heap.set(v);
          }
        }
      , val.handle()
      , p.data()
      );
    }
  };

  template <>
  struct convert_payload<json_payload>
  {
    static void apply(nbdl::detail::js_val const& val, json_payload& p)
    {
      EM_ASM_(
        {
          var v = Module.NBDL_DETAIL_JS_GET($0);
          if (typeof v === 'string')
          {
            Module.NBDL_DETAIL_JS_SET($1, JSON.parse(v));
          }
          else
          {
            throw 'JSON must be communicated as text.';
          }
        }
      , val.handle()
      , p.handle()
      );
    }
  };
}

#endif
