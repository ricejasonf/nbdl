//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_SERVERS_WS_MESSAGE_PARSER_HPP
#define NBDL_SERVERS_WS_MESSAGE_PARSER_HPP

#include<cstdint>

namespace nbdl {
namespace servers {
namespace ws {

class MessageParser
{
  public:

  enum struct Result {
    GOOD,
    BAD,
    PING,
    PONG,
    CLOSE,
    INDETERMINATE
  };

  private:

  bool expecting_binary;
  bool is_last_frame;
  uint64_t payload_length;
  uint64_t payload_pos;
  std::array<char, 4> mask_key;
  std::string body; //<-- use fixed array?
  //todo uint64_t max_message_size

  enum State {
    FRAME_HEADER,
    PAYLOAD_LENGTH, //also contains mask bit (must be set)
    EXTENDED_PAYLOAD_LENGTH_16BIT_1,
    EXTENDED_PAYLOAD_LENGTH_16BIT_2,
    EXTENDED_PAYLOAD_LENGTH_64BIT_1,
    EXTENDED_PAYLOAD_LENGTH_64BIT_2,
    EXTENDED_PAYLOAD_LENGTH_64BIT_3,
    EXTENDED_PAYLOAD_LENGTH_64BIT_4,
    EXTENDED_PAYLOAD_LENGTH_64BIT_5,
    EXTENDED_PAYLOAD_LENGTH_64BIT_6,
    EXTENDED_PAYLOAD_LENGTH_64BIT_7,
    EXTENDED_PAYLOAD_LENGTH_64BIT_8,
    MASK_KEY_1,
    MASK_KEY_2,
    MASK_KEY_3,
    MASK_KEY_4,
    READING_PAYLOAD,
    FINISHED
  } state;

  Result consume(char);
  template<State s, typename = void>
  Result consume_(char);
  Result applyToLength(char, int);
  Result applyToMaskKey(char, int);
  Result finish();

  public:

  MessageParser(bool use_binary = false) :
    expecting_binary(use_binary),
    is_last_frame(false),
    payload_length(0),
    payload_pos(0),
    state(FRAME_HEADER)
  {}

  enum struct Result {
    GOOD,
    BAD,
    INDETERMINATE
  };

	template<typename InputIterator>
	Result parse(InputIterator begin, InputIterator end)
	{
		while (begin != end)
		{
			Result r = consume(*begin++);
			if (r == GOOD || r == BAD)
				return r;	
		}
		return INDETERMINATE;
	}

};

}//ws
}//servers
}//nbdl

#endif
