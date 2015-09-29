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
    INDETERMINATE
  };

  private:

  bool is_last_frame;
  uint64_t payload_length;
  std::array<char, 4> mask_key;
  std::string body; //<-- use fixed array?
  //todo uint64_t max_message_size

  enum {
    FRAME_HEADER,
    PAYLOAD_LENGTH, //also contains mask bit (must be set)
    EXTENDED_PAYLOAD_LENGTH_1,
    EXTENDED_PAYLOAD_LENGTH_2,
    EXTENDED_PAYLOAD_LENGTH_3,
    EXTENDED_PAYLOAD_LENGTH_4,
    EXTENDED_PAYLOAD_LENGTH_5,
    EXTENDED_PAYLOAD_LENGTH_6,
    EXTENDED_PAYLOAD_LENGTH_7,
    EXTENDED_PAYLOAD_LENGTH_8,
    MASK_KEY_1,
    MASK_KEY_2,
    MASK_KEY_3,
    MASK_KEY_4,
    READING_PAYLOAD,
    FINISHED
  };

  Result consume(char);

  public:

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
