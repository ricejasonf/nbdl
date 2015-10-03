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

  int opcode;
  int payload_length;
  Buffer body;
  MaskKey mask_key;
  bool has_error;

  void generateHeader();
  void generatePayloadLength();
  void generateExtendedPayloadLength();
  void generateMask();
  void writeByte(int, char);
  char applyMaskBit(char);

  void initFrame(int length, int opcode_, MaskKey mask = Null{})
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
    generateHeader();
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
    generateHeader();
    if (has_error)
      return false;
		while (begin != end)
		{
      writeByte(pos++, *begin++);
		}
		return true;
	}

  public:

  const Buffer& getBuffer() const { return body; }

	template<typename InputIterator>
  bool generateText(InputIterator begin, InputIterator end, int length, MaskKey mask = Null{})
  {
    initFrame(length, 0x1, mask);
    return generate(begin, end);
  }
	template<typename InputIterator>
  bool generateBinary(InputIterator begin, InputIterator end, int length, MaskKey mask = Null{})
  {
    initFrame(length, 0x2, mask);
    return generate(begin, end);
  }
  bool generateClose()
  {
    initFrame(0, 0x8);
    return generate();
  }
  bool generatePing()
  {
    initFrame(0, 0x9);
    return generate();
  }
  bool generatePong()
  {
    initFrame(0, 0xA);
    return generate();
  }

  //the following are for testing the parser
	template<typename InputIterator>
  bool generateTextFragment(InputIterator begin, InputIterator end, int length,
      MaskKey mask = Null{})
  {
    initFrame(length, 0x1, mask);
    bool result = generate(begin, end);
    body[0] = 0x1; //no fin bit
    return result;
  }
	template<typename InputIterator>
  bool generateFinalContinuation(InputIterator begin, InputIterator end, int length,
      MaskKey mask = Null{})
  {
    initFrame(length, 0x0, mask);
    return generate(begin, end);
  }
	template<typename InputIterator>
  bool generateContinuation(InputIterator begin, InputIterator end, int length,
      MaskKey mask = Null{})
  {
    initFrame(length, 0x0, mask);
    bool result = generate(begin, end);
    body[0] = 0x0; //no fin bit
    return result;
  }
};

}//ws
}//servers
}//nbdl

#endif
