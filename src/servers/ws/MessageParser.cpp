//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "MessageParser.hpp"

namespace ws = nbdl::servers::ws;
using Result = ws::MessageParser::Result;

Result ws::MessageParser::consume(unsigned char c)
{
  switch(state)
  {
    case FRAME_HEADER:
      return consumeFrameHeader(c);
    case PAYLOAD_LENGTH:
      return consumePayloadLength(c);
    case EXTENDED_PAYLOAD_LENGTH_16BIT_1:
      return applyToLength(c, 0);
    case EXTENDED_PAYLOAD_LENGTH_16BIT_2:
      finishReadingLength();
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
      finishReadingLength();
      return applyToLength(c, 7);
    case MASK_KEY_1:
      state = MASK_KEY_2;
      return applyToMaskKey(c, 0);
    case MASK_KEY_2:
      state = MASK_KEY_3;
      return applyToMaskKey(c, 1);
    case MASK_KEY_3:
      state = MASK_KEY_4;
      return applyToMaskKey(c, 2);
    case MASK_KEY_4:
      if (payload_length == 0)
      {
        return finishFrame();
      }
      else
      {
        state = READING_PAYLOAD;
        return applyToMaskKey(c, 3);
      }
    case READING_PAYLOAD:
      return consumeReadingPayload(c);
    case FINISHED_MESSAGE:
      //start it all over anew
      //is clear the right choice here?
      body.clear();
      consumeFrameHeader(c);
  }
  return Result::BAD;
}

Result ws::MessageParser::applyToLength(unsigned char c, int i)
{
  payload_length |= (c << (8 * i));
}

Result ws::MessageParser::consumeFrameHeader(unsigned char c)
{
  unsigned char opcode = c & 0x0F;
  switch (opcode)
  {
    case 0x0:
    case 0x1:
    case 0x2:
      is_last_frame = (c >> 7) != 0;
      break;
    case 0x8:
    case 0x9:
    case 0xA:
      control_opcode = opcode;
      break;
    default:
      return Result::BAD;
  }
  state = PAYLOAD_LENGTH;
  return Result::INDETERMINATE;
}

Result ws::MessageParser::consumePayloadLength(unsigned char c)
{
  //mask bit must be set the 1
  has_mask = ((c >> 7) != 0);
  if (has_mask != require_mask)
    return Result::BAD;
  //clear the mask bit
  c &= 127;
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
      finishReadingLength();
      break;
  }
  return Result::INDETERMINATE;
}

void ws::MessageParser::finishReadingLength()
{
  if (has_mask)
    state = MASK_KEY_1;
  else
    state = READING_PAYLOAD;
}

Result ws::MessageParser::applyToMaskKey(unsigned char c, int i)
{
  mask_key[i] = c;
  return Result::INDETERMINATE;
}

Result ws::MessageParser::consumeReadingPayload(unsigned char c)
{
  if (!control_opcode)
  {
    body.push_back(c ^ mask_key[payload_pos % 4]);
    ++payload_pos;
  }
  if (payload_pos >= payload_length)
    return finishFrame();
  return Result::INDETERMINATE;
}

Result ws::MessageParser::finishFrame()
{
  if (control_opcode)
    return finishControlFrame();

  payload_pos = 0;
  if (is_last_frame)
  {
    state = FINISHED_MESSAGE;
    return Result::MESSAGE;
  }
  else
  {
    //need to read more frames to finish the message
    state = FRAME_HEADER;
    return Result::INDETERMINATE;
  }
  return Result::BAD;
}

Result ws::MessageParser::finishControlFrame()
{
  //setup to read next frame
  state = FRAME_HEADER;

  switch (control_opcode)
  {
    case 0x8:
      return Result::CLOSE;
    case 0x9:
      return Result::PING;
    case 0xA:
      return Result::PONG;
  }
  return Result::BAD;
}
