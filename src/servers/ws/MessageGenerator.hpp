//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_SERVERS_WS_MESSAGE_GENERATOR_HPP
#define NBDL_SERVERS_WS_MESSAGE_GENERATOR_HPP

#include<array>
#include<vector>
#include<cstdint>
#include "../../Variant.hpp"

namespace nbdl {
namespace servers {
namespace ws {

class message_generator
{
  public:

  using MaskKey = optional<std::array<char, 4>>;

  enum class result {
    UNFINISHED,
    FINISHED,
    ERROR
  };

  using Buffer = std::vector<char>;

  private:

  int opcode;
  int payload_length;
  Buffer body;
  MaskKey mask_key;
  bool has_error;

  void generate_header();
  void generate_payload_length();
  void generate_extended_payload_length();
  void generate_mask();
  void write_byte(int, char);
  char apply_mask_bit(char);

  void init_frame(int length, int opcode_, MaskKey mask = nothing{})
  {
    if (length < 0 || length > 65535)
      has_error = true;
    payload_length = length;
    opcode = opcode_;
    mask_key = mask;
  }

	bool generate()
  {
    if (body.size())
      body.clear();
    generate_header();
    if (has_error)
      return false;
    return true;
  }

	template<typename InputIterator>
	bool generate(InputIterator begin, InputIterator end)
	{
    int pos = 0;
    if (body.size())
      body.clear();
    generate_header();
    if (has_error)
      return false;
		while (begin != end)
		{
      write_byte(pos++, *begin++);
		}
		return true;
	}

  public:

  const Buffer& get_buffer() const { return body; }

	template<typename InputIterator>
  bool generate_text(InputIterator begin, InputIterator end, int length, MaskKey mask = nothing{})
  {
    init_frame(length, 0x1, mask);
    return generate(begin, end);
  }
	template<typename InputIterator>
  bool generate_binary(InputIterator begin, InputIterator end, int length, MaskKey mask = nothing{})
  {
    init_frame(length, 0x2, mask);
    return generate(begin, end);
  }
  bool generate_close()
  {
    init_frame(0, 0x8);
    return generate();
  }
  bool generate_ping()
  {
    init_frame(0, 0x9);
    return generate();
  }
  bool generate_pong()
  {
    init_frame(0, 0xA);
    return generate();
  }

  //the following are for testing the parser
	template<typename InputIterator>
  bool generate_text_fragment(InputIterator begin, InputIterator end, int length,
      MaskKey mask = nothing{})
  {
    init_frame(length, 0x1, mask);
    bool result = generate(begin, end);
    body[0] = 0x1; //no fin bit
    return result;
  }
	template<typename InputIterator>
  bool generate_final_continuation(InputIterator begin, InputIterator end, int length,
      MaskKey mask = nothing{})
  {
    init_frame(length, 0x0, mask);
    return generate(begin, end);
  }
	template<typename InputIterator>
  bool generate_continuation(InputIterator begin, InputIterator end, int length,
      MaskKey mask = nothing{})
  {
    init_frame(length, 0x0, mask);
    bool result = generate(begin, end);
    body[0] = 0x0; //no fin bit
    return result;
  }
};

}//ws
}//servers
}//nbdl

#endif
