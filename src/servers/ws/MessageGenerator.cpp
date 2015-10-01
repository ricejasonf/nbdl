//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "MessageGenerator.hpp"

namespace ws = nbdl::servers::ws;
using Result = ws::MessageGenerator::Result;

ws::MessageGenerator::MessageGenerator(int length, MaskKey mask, bool use_binary) :
  is_binary(use_binary),
  payload_length(length),
  payload_pos(0),
  body(length),
  mask_key(mask),
  has_error(false)
{
  if (length < 0 || length > 65536)
  {
    //don't support really large messages
    has_error = true;
    return;
  }
}

void ws::MessageGenerator::generateHeader()
{
  char opcode = is_binary ? 2 : 1;
  body.push_back(opcode | (1 << 7)); //apply FIN bit
  generatePayloadLength();
}

void ws::MessageGenerator::generatePayloadLength()
{
  if (payload_length > 126)
  {
    body.push_back(applyMaskBit(126));
    generateExtendedPayloadLength();
  }
  else
  {
    body.push_back(applyMaskBit(payload_length));
  }
}

void ws::MessageGenerator::generateExtendedPayloadLength()
{
  body.push_back(payload_length >> 8);
  body.push_back(payload_length);
}

char ws::MessageGenerator::applyMaskBit(char len)
{
  return mask_key.match(
    [&](Null) {
      return len;
    },
    [&]() {
      return len | (1 << 7);
    });
}

void ws::MessageGenerator::writeByte(int pos, char c)
{
  body.push_back(mask_key.match(
    [&](Null) {
      return c;
    },
    [&](std::array<char, 4> key) {
      return c ^ key[pos % 4]; 
    }));
}
