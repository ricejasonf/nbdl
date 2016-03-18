//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "MessageGenerator.hpp"

namespace ws = nbdl::servers::ws;
using Result = ws::message_generator::result;

void ws::message_generator::generate_header()
{
  body.push_back(opcode | (1 << 7)); //apply FIN bit
  generate_payload_length();
  generate_mask();
}

void ws::message_generator::generate_payload_length()
{
  if (payload_length > 126)
  {
    body.push_back(apply_mask_bit(126));
    generate_extended_payload_length();
  }
  else
  {
    body.push_back(apply_mask_bit(payload_length));
  }
}

void ws::message_generator::generate_extended_payload_length()
{
  body.push_back(payload_length >> 8);
  body.push_back(payload_length);
}

char ws::message_generator::apply_mask_bit(char len)
{
  return mask_key.match(
    [&](nothing) {
      return len;
    },
    [&](auto) {
      return len | (1 << 7);
    });
}

void ws::message_generator::generate_mask()
{
  mask_key.match(
    [&](std::array<char, 4> key) {
      for (int i = 0; i < 4; i++)
      {
        body.push_back(key[i]);
      }
    }, noop);
}

void ws::message_generator::write_byte(int pos, char c)
{
  body.push_back(mask_key.match(
    [&](nothing) {
      return c;
    },
    [&](std::array<char, 4> key) {
      return c ^ key[pos % 4]; 
    }));
}
