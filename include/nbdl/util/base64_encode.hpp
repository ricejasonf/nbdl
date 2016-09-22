//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_UTIL_BASE64_ENCODE_HPP
#define NBDL_UTIL_BASE64_ENCODE_HPP

#include <array>
#include <string>

namespace nbdl::util
{
  namespace base64_detail
  {
    static constexpr char const* lookup =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz"
      "0123456789+/";

    static constexpr int const offsets[4]{ 26, 20, 14, 8 };

    template <typename Output>
    void encode_fragment(Output& output, uint32_t fragment, int byte_length)
    {
      if (byte_length == 0) return;
      for (int i = 0; i <= byte_length; i++)
      {
        output.push_back(lookup[(fragment & (0x3F << offsets[i])) >> offsets[i]]);
      }
    }
  }

  struct base64_encode_fn
  {
    template <typename Buffer>
    std::string operator()(Buffer&& buffer) const
    {
      using buffer_value_t = typename std::decay_t<Buffer>::value_type;
      static_assert(sizeof(buffer_value_t) == 1);
      static_assert(sizeof(uint32_t) == 4);

      std::string output;
      output.reserve(buffer.size() * (3 / 4));

      int const length          = buffer.size();
      int const extra_length    = length % 3;
      int const round_length    = length - extra_length;
      int const padding_length  = (3 - extra_length) % 3;

      for (int i = 0; i < round_length; i += 3)
      {
        uint32_t const fragment =
            buffer[i    ] << 24
          | buffer[i + 1] << 16
          | buffer[i + 2] << 8
          ;
        base64_detail::encode_fragment(output, fragment, 3);
      }

      uint32_t extras = 0;
      for (int i = 0; i < extra_length; i++)
      {
        extras |= buffer[round_length + i] << (8 * (3 - i));
      }
      base64_detail::encode_fragment(output, extras, extra_length);

      for (int i = 0; i < padding_length; i++)
      {
        output.push_back('=');
      }

      return output;
    }
  };

  constexpr base64_encode_fn base64_encode{};
}
#endif
