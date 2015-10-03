//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include<array>

#include<servers/ws/MessageParser.hpp>
#include<servers/ws/MessageGenerator.hpp>
#include "../catch.hpp"

TEST_CASE("Create/Parse message in a single, short, unmasked data frame.", "[websocket]") 
{
	using MessageParser = nbdl::servers::ws::MessageParser;
	using MessageGenerator = nbdl::servers::ws::MessageGenerator;
  const std::string message = "Hello world!";

  auto gen = MessageGenerator();
  gen.generateText(message.begin(), message.end(), message.size());
  auto gen_buffer = gen.getBuffer();

	MessageParser parser;	
	auto result = parser.parse(gen_buffer.begin(), gen_buffer.end());


	REQUIRE(result == MessageParser::Result::MESSAGE);
  auto buffer = parser.getBuffer();
  std::string echoed_message(buffer.begin(), buffer.end());

	CHECK(message == echoed_message);
}

TEST_CASE("Create/Parse message in a single, short, masked data frame.", "[websocket]") 
{
	using MessageParser = nbdl::servers::ws::MessageParser;
	using MessageGenerator = nbdl::servers::ws::MessageGenerator;
  const std::string message = "Hello world!";

  std::array<char, 4> mask = { 4, 5, 6, 7 };
  auto gen = MessageGenerator();
  gen.generateText(message.begin(), message.end(), message.size(), mask);
  auto gen_buffer = gen.getBuffer();

	MessageParser parser = MessageParser(true);	
	auto result = parser.parse(gen_buffer.begin(), gen_buffer.end());

	REQUIRE(result == MessageParser::Result::MESSAGE);
  auto buffer = parser.getBuffer();
  std::string echoed_message(buffer.begin(), buffer.end());

	CHECK(message == echoed_message);
}

TEST_CASE("Parse fragmented message with interjecting control frame.", "[websocket]") 
{
	using MessageParser = nbdl::servers::ws::MessageParser;
	using MessageGenerator = nbdl::servers::ws::MessageGenerator;
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
  const auto& buf = gen.getBuffer();
  gen.generateTextFragment(message.begin(), message.end(), message.size());
  frames.push_back(buf);
  gen.generateContinuation(message.begin(), message.end(), message.size());
  frames.push_back(buf);
  gen.generateContinuation(message.begin(), message.end(), message.size());
  frames.push_back(buf);
  gen.generatePing();
  frames.push_back(buf);
  gen.generateContinuation(message.begin(), message.end(), message.size());
  frames.push_back(buf);
  gen.generateFinalContinuation(message.begin(), message.end(), message.size());
  frames.push_back(buf);

  auto parser = MessageParser();
  MessageGenerator::Buffer frame;
  int i = 0;
  frame = frames[i++];
	CHECK(parser.parse(frame.begin(), frame.end())
        == MessageParser::Result::INDETERMINATE);
  frame = frames[i++];
	CHECK(parser.parse(frame.begin(), frame.end())
        == MessageParser::Result::INDETERMINATE);
  frame = frames[i++];
	CHECK(parser.parse(frame.begin(), frame.end())
        == MessageParser::Result::INDETERMINATE);
  frame = frames[i++];
	CHECK(parser.parse(frame.begin(), frame.end())
        == MessageParser::Result::PING);
  frame = frames[i++];
	CHECK(parser.parse(frame.begin(), frame.end())
        == MessageParser::Result::INDETERMINATE);
  frame = frames[i++];
	REQUIRE(parser.parse(frame.begin(), frame.end())
        == MessageParser::Result::MESSAGE);

  auto buffer = parser.getBuffer();
  std::string echoed_message(buffer.begin(), buffer.end());

	CHECK(expected_result == echoed_message);
}
