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

class MessageGenerator
{
  public:

  using MaskKey = Optional<std::array<char, 4>>;

  enum class Result {
    UNFINISHED,
    FINISHED,
    ERROR
  };

  using Buffer = std::vector<char>;

  private:

  bool is_binary;
  int payload_length;
  int payload_pos;
  Buffer body;
  MaskKey mask_key;
  bool has_error;

  void generateHeader();
  void generatePayloadLength();
  void generateExtendedPayloadLength();
  void generateMask();
  void writeByte(int, char);
  char applyMaskBit(char);

  public:

  MessageGenerator(int length, MaskKey mask = Null{}, bool use_binary = false);

  const Buffer& getBuffer() const { return body; }

	template<typename InputIterator>
	bool generate(InputIterator begin, InputIterator end)
	{
    int pos = 0;
    body.clear();
    generateHeader();
    if (has_error)
      return false;
		while (begin != end)
		{
      writeByte(pos++, *begin++);
		}
		return true;
	}

};

}//ws
}//servers
}//nbdl

#endif
