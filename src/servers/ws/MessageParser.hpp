//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_SERVERS_WS_MESSAGE_PARSER_HPP
#define NBDL_SERVERS_WS_MESSAGE_PARSER_HPP

#include<array>
#include<vector>
#include<cstdint>

namespace nbdl {
namespace servers {
namespace ws {

class message_parser
{
  public:

  enum struct result {
    INDETERMINATE,
    MESSAGE,
    PING,
    PONG,
    CLOSE,
    BAD,
  };

  using Buffer = std::vector<char>;

  private:

  bool require_mask;
  bool has_mask;
  bool expecting_binary;
  bool is_last_frame;
  uint64_t payload_length;
  uint64_t payload_pos;
  std::array<char, 4> mask_key;
  char control_opcode;
  Buffer body;
  //todo uint64_t max_message_size

  enum state {
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
    FINISHED_MESSAGE
  } state;

  result consume(unsigned char);
  result consume_frame_header(unsigned char);
  result consume_payload_length(unsigned char);
  result consume_reading_payload(unsigned char);
  result apply_to_length(unsigned char, int);
  result apply_to_mask_key(unsigned char, int);
  void finish_reading_length();
  result finish_frame();
  result finish_control_frame();

  public:

  message_parser(bool req_mask = false, bool use_binary = false) :
    require_mask(req_mask),
    expecting_binary(use_binary),
    is_last_frame(false),
    payload_length(0),
    payload_pos(0),
    mask_key(),
    control_opcode(0),
    state(FRAME_HEADER)
  { (void)expecting_binary; }

  const Buffer& get_buffer() const { return body; }

	template<typename InputIterator>
	result parse(InputIterator begin, InputIterator end)
	{
    //todo handle control frames
		while (begin != end)
		{
			result r = consume(*begin++);
			if (r != result::INDETERMINATE)
				return r;	
		}
		return result::INDETERMINATE;
	}

};

}//ws
}//servers
}//nbdl

#endif
