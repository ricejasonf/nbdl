//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_UTIL_BASE64_ENCODE_HPP
#define NBDL_UTIL_BASE64_ENCODE_HPP

#include <array>
#include <cassert>
#include <iterator>
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

    template <typename OutputItr>
    void encode_fragment(OutputItr& output_itr, uint32_t fragment, int byte_length)
    {
      if (byte_length == 0) return;
      for (int i = 0; i <= byte_length; i++)
      {
        *output_itr = lookup[(fragment & (0x3F << offsets[i])) >> offsets[i]];
        ++output_itr;
      }
    }

    inline constexpr int encoding_length_padded(int n)
    {
      return 4*n/3 + ((4 - ((4*n/3) % 4)) % 4);
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

      std::string output(base64_detail::encoding_length_padded(buffer.size()), '=');
      auto output_itr = output.begin();

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
        base64_detail::encode_fragment(output_itr, fragment, 3);
      }

      uint32_t extras = 0;
      for (int i = 0; i < extra_length; i++)
      {
        extras |= buffer[round_length + i] << (8 * (3 - i));
      }
      base64_detail::encode_fragment(output_itr, extras, extra_length);

      assert(output_itr + padding_length == output.end());

      return output;
    }
  };

  constexpr base64_encode_fn base64_encode{};
}
#endif
