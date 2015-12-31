//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include<servers/ws/HttpResponse.hpp>
#include<catch.hpp>

using HttpRequest = nbdl::servers::ws::HttpRequest;
using HttpResponse = nbdl::servers::ws::HttpResponse;

TEST_CASE("Generated Response should contain the proper Sec-WebSocket-Accept string.", "[websocket]") 
{
	const char* expected = "HTTP/1.1 101 Switching Protocols"
		"\r\n"
		"Upgrade: websocket"
		"\r\n"
		"Connection: Upgrade"
		"\r\n"
		"Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo="
		"\r\n"
		"\r\n"
		;
	HttpRequest request;
	request.sec_websocket_key_header = "dGhlIHNhbXBsZSBub25jZQ==";

	auto response = HttpResponse(request);
	CHECK(response.response == expected);
}
