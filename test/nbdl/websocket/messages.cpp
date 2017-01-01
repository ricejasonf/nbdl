//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <servers/ws/MessageParser.hpp>
#include <servers/ws/MessageGenerator.hpp>

#include <array>
#include <catch.hpp>
#include <string>

TEST_CASE("Create/Parse message in a single, short, unmasked data frame.", "[websocket]") 
{
	using MessageParser = nbdl::servers::ws::message_parser;
	using MessageGenerator = nbdl::servers::ws::message_generator;
  const std::string message = "Hello world!";

  auto gen = MessageGenerator();
  gen.generate_text(message.begin(), message.end(), message.size());
  auto gen_buffer = gen.get_buffer();

	MessageParser parser;	
	auto result = parser.parse(gen_buffer.begin(), gen_buffer.end());


	REQUIRE(result == MessageParser::result::MESSAGE);
  auto buffer = parser.get_buffer();
  std::string echoed_message(buffer.begin(), buffer.end());

	CHECK(message == echoed_message);
}

TEST_CASE("Create/Parse message in a single, short, masked data frame.", "[websocket]") 
{
	using MessageParser = nbdl::servers::ws::message_parser;
	using MessageGenerator = nbdl::servers::ws::message_generator;
  const std::string message = "Hello world!";

  std::array<char, 4> mask = {{ 4, 5, 6, 7 }};
  auto gen = MessageGenerator();
  gen.generate_text(message.begin(), message.end(), message.size(), mask);
  auto gen_buffer = gen.get_buffer();

	MessageParser parser = MessageParser(true);	
	auto result = parser.parse(gen_buffer.begin(), gen_buffer.end());

	REQUIRE(result == MessageParser::result::MESSAGE);
  auto buffer = parser.get_buffer();
  std::string echoed_message(buffer.begin(), buffer.end());

	CHECK(message == echoed_message);
}

TEST_CASE("Parse fragmented message with interjecting control frame.", "[websocket]") 
{
	using MessageParser = nbdl::servers::ws::message_parser;
	using MessageGenerator = nbdl::servers::ws::message_generator;
  const std::string message = "Hello world!";
  const char* expected_result = 
    "Hello world!"
    "Hello world!"
    "Hello world!"
    "Hello world!"
    "Hello world!"
    ;
  std::vector<MessageGenerator::Buffer> frames;

  auto gen = MessageGenerator();
  const auto& buf = gen.get_buffer();
  gen.generate_text_fragment(message.begin(), message.end(), message.size());
  frames.push_back(buf);
  gen.generate_continuation(message.begin(), message.end(), message.size());
  frames.push_back(buf);
  gen.generate_continuation(message.begin(), message.end(), message.size());
  frames.push_back(buf);
  gen.generate_ping();
  frames.push_back(buf);
  gen.generate_continuation(message.begin(), message.end(), message.size());
  frames.push_back(buf);
  gen.generate_final_continuation(message.begin(), message.end(), message.size());
  frames.push_back(buf);

  auto parser = MessageParser();
  MessageGenerator::Buffer frame;
  int i = 0;
  frame = frames[i++];
	CHECK(parser.parse(frame.begin(), frame.end())
        == MessageParser::result::INDETERMINATE);
  frame = frames[i++];
	CHECK(parser.parse(frame.begin(), frame.end())
        == MessageParser::result::INDETERMINATE);
  frame = frames[i++];
	CHECK(parser.parse(frame.begin(), frame.end())
        == MessageParser::result::INDETERMINATE);
  frame = frames[i++];
	CHECK(parser.parse(frame.begin(), frame.end())
        == MessageParser::result::PING);
  frame = frames[i++];
	CHECK(parser.parse(frame.begin(), frame.end())
        == MessageParser::result::INDETERMINATE);
  frame = frames[i++];
	REQUIRE(parser.parse(frame.begin(), frame.end())
        == MessageParser::result::MESSAGE);

  auto buffer = parser.get_buffer();
  std::string echoed_message(buffer.begin(), buffer.end());

	CHECK(expected_result == echoed_message);
}
