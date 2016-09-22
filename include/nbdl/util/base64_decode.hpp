//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_UTIL_BASE64_DECODE_HPP
#define NBDL_UTIL_BASE64_DECODE_HPP

#include <nbdl/util/base64_encode.hpp>

#include <array>
#include <vector>

namespace nbdl::util
{
  namespace base64_detail
  {
    static constexpr unsigned char const PADDING = 65;
    static constexpr unsigned char const INVALID = 64;

    static unsigned char const reverse_lookup[] = {
      64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
      64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
  //                                    +           /
      64,64,64,64,64,64,64,64,64,64,64,62,64,64,64,63,
  //   0  1  2  3  4  5  6  7  8  9           =
      52,53,54,55,56,57,58,59,60,61,64,64,64,65,64,64,
  //      A  B  C  D  E  F  G  H  I  J  K  L  M  N  O
      64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
  //   P  Q  R  S  T  U  V  W  X  Y  Z
      15,16,17,18,19,20,21,22,23,24,25,64,64,64,64,64,
  //      a  b  c  d  e  f  g  h  i  j  k  l  m  n  o
      64,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
  //   p  q  r  s  t  u  v  w  x  y  z
      41,42,43,44,45,46,47,48,49,50,51,64,64,64,64,64,
      64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
      64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
      64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
      64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
      64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
      64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
      64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
      64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64
    };
  }

  struct base64_decode_fn
  {
    using output_t = std::vector<unsigned char>;

    template <typename InputBuffer>
    output_t operator()(InputBuffer&& buffer) const
    {
      using buffer_value_t = typename std::decay_t<InputBuffer>::value_type;
      static_assert(sizeof(buffer_value_t) == 1);

      using base64_detail::reverse_lookup;
      int const length = buffer.size();
      output_t output{};

      if (length % 4 != 0)
      {
        return output;
      }

      int j;
      for (int i = 0; i < length; i += 4)
      {
        uint32_t fragment = 0;
        for (j = 0; j < 4; )
        {
          unsigned char value = reverse_lookup[static_cast<unsigned char>(buffer[i + j])];
          if (value == base64_detail::PADDING)
          {
            break;
          }
          if (value == base64_detail::INVALID)
          {
            // return empty buffer
            return output_t{};
          }
          fragment |= value << base64_detail::offsets[j];
          ++j;
        }
        static int const length_map[]{ 3, 2, 2, 1, 0 };
        for (int k = 3; k > length_map[j]; k--)
        {
          output.push_back(fragment >> (k * 8));
        }
      }
      return output;
    }
  };

  constexpr base64_decode_fn base64_decode{};
}
#endif
