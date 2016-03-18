//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "MessageParser.hpp"

namespace ws = nbdl::servers::ws;
using Result = ws::message_parser::result;

Result ws::message_parser::consume(unsigned char c)
{
  switch(state)
  {
    case FRAME_HEADER:
      return consume_frame_header(c);
    case PAYLOAD_LENGTH:
      return consume_payload_length(c);
    case EXTENDED_PAYLOAD_LENGTH_16BIT_1:
      return apply_to_length(c, 0);
    case EXTENDED_PAYLOAD_LENGTH_16BIT_2:
      finish_reading_length();
      return apply_to_length(c, 1);
    case EXTENDED_PAYLOAD_LENGTH_64BIT_1:
      return apply_to_length(c, 0);
    case EXTENDED_PAYLOAD_LENGTH_64BIT_2:
      return apply_to_length(c, 1);
    case EXTENDED_PAYLOAD_LENGTH_64BIT_3:
      return apply_to_length(c, 2);
    case EXTENDED_PAYLOAD_LENGTH_64BIT_4:
      return apply_to_length(c, 3);
    case EXTENDED_PAYLOAD_LENGTH_64BIT_5:
      return apply_to_length(c, 4);
    case EXTENDED_PAYLOAD_LENGTH_64BIT_6:
      return apply_to_length(c, 5);
    case EXTENDED_PAYLOAD_LENGTH_64BIT_7:
      return apply_to_length(c, 6);
    case EXTENDED_PAYLOAD_LENGTH_64BIT_8:
      finish_reading_length();
      return apply_to_length(c, 7);
    case MASK_KEY_1:
      state = MASK_KEY_2;
      return apply_to_mask_key(c, 0);
    case MASK_KEY_2:
      state = MASK_KEY_3;
      return apply_to_mask_key(c, 1);
    case MASK_KEY_3:
      state = MASK_KEY_4;
      return apply_to_mask_key(c, 2);
    case MASK_KEY_4:
      if (payload_length == 0)
      {
        return finish_frame();
      }
      else
      {
        state = READING_PAYLOAD;
        return apply_to_mask_key(c, 3);
      }
    case READING_PAYLOAD:
      return consume_reading_payload(c);
    case FINISHED_MESSAGE:
      //start it all over anew
      //is clear the right choice here?
      body.clear();
      consume_frame_header(c);
  }
  return Result::BAD;
}

Result ws::message_parser::apply_to_length(unsigned char c, int i)
{
  payload_length |= (c << (8 * i));
  return Result::INDETERMINATE;
}

Result ws::message_parser::consume_frame_header(unsigned char c)
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

Result ws::message_parser::consume_payload_length(unsigned char c)
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
      if (payload_length == 0)
        return finish_frame();
      finish_reading_length();
      break;
  }
  return Result::INDETERMINATE;
}

void ws::message_parser::finish_reading_length()
{
  if (has_mask)
    state = MASK_KEY_1;
  else
    state = READING_PAYLOAD;
}

Result ws::message_parser::apply_to_mask_key(unsigned char c, int i)
{
  mask_key[i] = c;
  return Result::INDETERMINATE;
}

Result ws::message_parser::consume_reading_payload(unsigned char c)
{
  if (!control_opcode)
  {
    body.push_back(c ^ mask_key[payload_pos % 4]);
    ++payload_pos;
  }
  if (payload_pos >= payload_length)
    return finish_frame();
  return Result::INDETERMINATE;
}

Result ws::message_parser::finish_frame()
{
  if (control_opcode)
    return finish_control_frame();

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

Result ws::message_parser::finish_control_frame()
{
  //setup to read next frame
  state = FRAME_HEADER;

  Result result;
  switch (control_opcode)
  {
    case 0x8:
      result = Result::CLOSE;
      break;
    case 0x9:
      result = Result::PING;
      break;
    case 0xA:
      result = Result::PONG;
      break;
    default:
      result = Result::BAD;
      break;
  }
  control_opcode = 0;
  return result;
}
