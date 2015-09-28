//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include<servers/ws/HttpRequestParser.hpp>
#include "../catch.hpp"

std::string request1 =
	"GET /chat HTTP/1.1"
	"\r\n"
	"Host: example.com:8000"
	"\r\n"
	"Origin: http://myorigin.com"
	"\r\n"
	"Upgrade: websocket"
	"\r\n"
	"Connection: Upgrade"
	"\r\n"
	"Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ=="
	"\r\n"
	"Sec-WebSocket-Version: 13"
	"\r\n"
	"Cookies: nbdl-auth:adslkjfasdf; blah=asdfasd;"
	"\r\n"
	"\r\n"
	;

TEST_CASE("Request parser should populate pertinent headers.", "[websocket]") 
{
	using HttpRequestParser = nbdl::servers::ws::HttpRequestParser;
	using HttpRequest = nbdl::servers::ws::HttpRequest;
	HttpRequestParser parser;	

	auto result = parser.parse(request1.begin(), request1.end());
	REQUIRE(result == HttpRequestParser::GOOD);

	HttpRequest request = parser.getRequest();
	CHECK(request.uri == "/chat");
	CHECK(request.upgrade_header == "websocket");
	CHECK(request.origin_header == "http://myorigin.com");
	CHECK(request.connection_header == "Upgrade");
	CHECK(request.sec_websocket_key_header == "dGhlIHNhbXBsZSBub25jZQ==");
	CHECK(request.sec_websocket_version_header == "13");
	CHECK(request.cookies_header == "nbdl-auth:adslkjfasdf; blah=asdfasd;");
}
