//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_UTIL_BASE64_DECODE_HPP
#define NBDL_UTIL_BASE64_DECODE_HPP

#include <nbdl/concept/DynamicBuffer.hpp>
#include <nbdl/util/base64_encode.hpp>

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

    constexpr auto is_invalid     = [](unsigned char c) { return reverse_lookup[c] == INVALID; };
    constexpr auto is_not_padding = [](unsigned char c) { return reverse_lookup[c] != PADDING; };

    template <typename Buffer>
    bool validate(Buffer const& b)
    {
      if (b.size() % 4 != 0) return false;
      if (std::any_of(b.begin(), b.end(), is_invalid)) return false;

      auto pad_itr = std::find(b.begin(), b.end(), '=');

      // no more than 2 PADDING bytes
      if (std::distance(pad_itr, b.end()) > 2) return false;

      // not non-PADDING bytes after first PADDING byte
      if (std::any_of(pad_itr, b.end(), is_not_padding)) return false;

      return true;
    }

    template <typename Buffer>
    auto source_length_unpadded(Buffer const& b)
    {
      return std::count_if(b.begin(), b.end(), is_not_padding);
    }

    template <typename Buffer>
    auto target_decoded_length(Buffer const& b)
    {
      int n = source_length_unpadded(b);
      return (3 * n) / 4;
    }

    template <DynamicBuffer Buffer>
    constexpr bool prepare_output_buffer(Buffer& buffer, int length) {
      buffer.resize(length);
      return true;
    }

    template <typename Buffer>
    constexpr bool prepare_output_buffer(Buffer& buffer, int length)
    {
      return buffer.size() == static_cast<std::size_t>(length);
    }

    template <typename Input, typename Output>
    bool decode(Input&& input, Output& output)
    {
      using buffer_value_t = typename std::decay_t<Input>::value_type;
      static_assert(sizeof(buffer_value_t) == 1);

      using base64_detail::reverse_lookup;

      int const length = source_length_unpadded(input);
      int const round_length = length - (length % 4);
      int const extra_length = length % 4;

      auto output_itr = output.begin();
      int i;
      for (i = 0; i < round_length; i += 4)
      {
        uint32_t fragment = 0;

        for (int j = 0; j < 4; j++)
        {
          unsigned char value = reverse_lookup[static_cast<unsigned char>(input[i + j])];
          fragment |= value << base64_detail::offsets[j];
        }

        for (int k = 3; k > 0; k--)
        {
          *output_itr = fragment >> (k * 8);
          ++output_itr;
        }
      }

      if (extra_length > 0)
      {
        uint32_t fragment = 0;

        for (int j = 0; j < extra_length; j++)
        {
          unsigned char value = reverse_lookup[static_cast<unsigned char>(input[i + j])];
          fragment |= value << base64_detail::offsets[j];
        }

        static int const length_map[]{3, 2, 2, 1, 0};
        for (int k = 3; k > length_map[extra_length]; k--)
        {
          *output_itr = fragment >> (k * 8);
          ++output_itr;
        }
      }

      assert(output_itr == output.end());

      return true;
    }
  }

  struct base64_decode_fn
  {
    using default_output_t = std::vector<unsigned char>;

    template <typename Input>
    default_output_t operator()(Input&& input) const
    {
      if (input.size() == 0) return default_output_t{};
      if (not base64_detail::validate(input)) return default_output_t{};

      default_output_t output(base64_detail::target_decoded_length(input), '?');

      return base64_detail::decode(std::forward<Input>(input), output) ? output : default_output_t{};
    }

    template <typename Input, typename Output>
    bool operator()(Input&& input, Output& output) const
    {
      using base64_detail::prepare_output_buffer;
      using base64_detail::target_decoded_length;
      using base64_detail::validate;
      using base64_detail::decode;

      if (not prepare_output_buffer(output, target_decoded_length(input))) return false;
      if (output.size() == 0) return true;
      if (not validate(input)) return false;

      return decode(std::forward<Input>(input), output);
    }
  };

  constexpr base64_decode_fn base64_decode{};
}
#endif
