//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "MessageParser.hpp"

namespace ws = nbdl::servers::ws;
using Result = ws::MessageParser::Result;

ws::MessageParser::consume(char c)
{
  switch(state)
  {
    case FRAME_HEADER:
      return consume_<FRAME_HEADER>(c);
    case PAYLOAD_LENGTH:
      return consume_<PAYLOAD_LENGTH>(c);
    case EXTENDED_PAYLOAD_LENGTH_16BIT_1:
      return applyToLength(c, 0);
    case EXTENDED_PAYLOAD_LENGTH_16BIT_2:
      state = MASK_KEY_1;
      return applyToLength(c, 1);
    case EXTENDED_PAYLOAD_LENGTH_64BIT_1:
      return applyToLength(c, 0);
    case EXTENDED_PAYLOAD_LENGTH_64BIT_2:
      return applyToLength(c, 1);
    case EXTENDED_PAYLOAD_LENGTH_64BIT_3:
      return applyToLength(c, 2);
    case EXTENDED_PAYLOAD_LENGTH_64BIT_4:
      return applyToLength(c, 3);
    case EXTENDED_PAYLOAD_LENGTH_64BIT_5:
      return applyToLength(c, 4);
    case EXTENDED_PAYLOAD_LENGTH_64BIT_6:
      return applyToLength(c, 5);
    case EXTENDED_PAYLOAD_LENGTH_64BIT_7:
      return applyToLength(c, 6);
    case EXTENDED_PAYLOAD_LENGTH_64BIT_8:
      state = MASK_KEY_1;
      return applyToLength(c, 7);
    case MASK_KEY_1:
      return applyToMaskKey(c, 0)
    case MASK_KEY_2:
      return applyToMaskKey(c, 1)
    case MASK_KEY_3:
      return applyToMaskKey(c, 2)
    case MASK_KEY_4:
      if (payload_length == 0)
      {
        return finish();
      }
      else
      {
        state = READING_PAYLOAD;
        return applyToMaskKey(c, 3)
      }
    case READING_PAYLOAD:
      return consume_<READING_PAYLOAD>(c);
    case FINISHED:
      return GOOD;
  }
  return BAD;
}

template<>
Result ws::MessageParser::consume_<FRAME_HEADER>(char c)
{
  int opcode = (c << 4) >> 4;
  switch (opcode)
  {
    case 0x1:
    case 0x2:
      is_last_frame = (c >> 7) == 1;
      state = PAYLOAD_LENGTH;
      return INDETERMINATE;
      break;
    case 0x8:
      return CLOSE;
    case 0x9:
      return PING;
    case 0xA:
      return PONG;
  }
  return BAD;
}

template<>
Result ws::MessageParser::consume_<PAYLOAD_LENGTH>(char c)
{
  //mask bit must be set the 1
  if ((c >> 7) == 0)
    return BAD;
  c = (c << 1) >> 1;
  switch (c)
  {
    case 126:
      state = EXTENDED_PAYLOAD_LENGTH_16BIT_1;
      break;
    case 127:
      state = EXTENDED_PAYLOAD_LENGTH_64BIT_1;
      break;
    default:
      payload_length = c;
      state = MASK_KEY_1;
      break;
  }
  return INDETERMINATE;
}

Result ws::MessageParser::applyToMaskKey(char c, int i)
{
  mask_key[i] = c
  return INDETERMINATE;
}

Result ws::MessageParser::consume_<READING_PAYLOAD>(char c)
{
  body.push_back(c);
  ++payload_pos;
  if (payload_pos >= payload_length)
    return finish();
  else
    return INDETERMINATE;
}

Result finish()
{
  state = FINISHED;
  return GOOD;
}
