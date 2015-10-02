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

  static MessageGenerator createText(int length, MaskKey mask = Null{})
  {
    return MessageGenerator(length, 0x1, mask);
  }
  static MessageGenerator createBinary(int length, MaskKey mask = Null{})
  {
    return MessageGenerator(length, 0x2, mask);
  }
  static MessageGenerator createClose()
  {
    return MessageGenerator(0, 0x8);
  }
  static MessageGenerator createPing()
  {
    return MessageGenerator(0, 0x9);
  }
  static MessageGenerator createPong()
  {
    return MessageGenerator(0, 0xA);
  }

  //used only for testing the parser
  static Buffer createTextFragment(std::string m, MaskKey mask = Null{})
  {
    auto g = MessageGenerator(m.size(), 0x1, mask);
    g.generate(m.begin(), m.end());
    Buffer buf = g.getBuffer();
    buf[0] = 0x1; //no fin bit
    return buf;
  }
  static Buffer createFinalContinuation(std::string m, MaskKey mask = Null{})
  {
    auto g = MessageGenerator(m.size(), 0x0, mask);
    g.generate(m.begin(), m.end());
    return g.getBuffer();
  }
  static Buffer createContinuation(std::string m, MaskKey mask = Null{})
  {
    auto g = MessageGenerator(m.size(), 0x0, mask);
    g.generate(m.begin(), m.end());
    Buffer buf = g.getBuffer();
    buf[0] = 0x0; //no fin bit
    return buf;
  }

  MessageGenerator(int length, int opcode = 0x1, MaskKey mask = Null{});

  const Buffer& getBuffer() const { return body; }

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

};

}//ws
}//servers
}//nbdl

#endif
