//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include<servers/ws/MessageParser.hpp>
#include<servers/ws/MessageGenerator.hpp>
#include "../catch.hpp"

TEST_CASE("Create/Parse message in a single short data frame.", "[websocket]") 
{
	using MessageParser = nbdl::servers::ws::MessageParser;
	using MessageGenerator = nbdl::servers::ws::MessageGenerator;
  const std::string message = "Hello world!";

	MessageGenerator gen(message.size());	
  gen.generate(message.begin(), message.end());
  auto gen_buffer = gen.getBuffer();

	MessageParser parser;	
	auto result = parser.parse(gen_buffer.begin(), gen_buffer.end());

  std::string echoed_message;

	REQUIRE(result == MessageParser::Result::GOOD);
  auto buffer = parser.getBuffer();
  for (auto c : buffer)
  {
    echoed_message.push_back(c);
  }

	CHECK(message == echoed_message);
}
