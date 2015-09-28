//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_SERVERS_WS_HTTP_RESPONSE_HPP
#define NBDL_SERVERS_WS_HTTP_RESPONSE_HPP

#include<string>
#include "HttpRequestParser.hpp"

namespace nbdl {
namespace servers {
namespace ws {

class HttpResponse
{
	void generateAcceptToken(const std::string& key);

	static constexpr const char* response_start = "HTTP/1.1 101 Switching Protocols"
		"\r\n"
		"Upgrade: websocket"
		"\r\n"
		"Connection: Upgrade"
		"\r\n"
		"Sec-WebSocket-Accept: ";

	public:

	std::string response;

	HttpResponse(const HttpRequest& req)
		: response(response_start)
	{
		generateAcceptToken(req.sec_websocket_key_header);
		response += "\r\n\r\n";
	}
};

}//ws
}//servers
}//nbdl

#endif
