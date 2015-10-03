//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "MessageGenerator.hpp"

namespace ws = nbdl::servers::ws;
using Result = ws::MessageGenerator::Result;

void ws::MessageGenerator::generateHeader()
{
  body.push_back(opcode | (1 << 7)); //apply FIN bit
  generatePayloadLength();
  generateMask();
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

void ws::MessageGenerator::generateMask()
{
  mask_key.match(
    [&](std::array<char, 4> key) {
      for (int i = 0; i < 4; i++)
      {
        body.push_back(key[i]);
      }
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
