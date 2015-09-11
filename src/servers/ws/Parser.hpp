//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_SERVERS_WS_PARSER_HPP
#define NBDL_SERVERS_WS_PARSER_HPP

#include "servers/ws/HttpRequest.hpp"

namespace nbdl {
namespace servers {
namespace ws {


class HttpRequestParser
{
	enum State {
		METHOD_GET_1,
		METHOD_GET_2,
		METHOD_GET_3,
		URI,
		HTTP_1,
		HTTP_2,
		HTTP_3,
		HTTP_4,
		HTTP_SLASH,
		HTTP_VERSION_MAJOR,
		HTTP_VERSION_POINT,
		HTTP_VERSION_MINOR,
		REQUEST_LINE_CR,
		REQUEST_LINE_LF,
		HEADER_START, //<-- to FINAL_LF if CR
		HEADER_NAME,
		HEADER_VALUE_START, //<-- ignore lws
		HEADER_VALUE,
		HEADER_LF,
		FINAL_LF,
		AFTER_FINAL_LF // <-- ignore body or 400??
	};

	State state_;

	Result consume(char c);

	Result requireChar(char l, char r, State next);

	static const int max_uri_length = 100;

	public:

	enum Result {
		INDETERMINATE,
		GOOD,
		BAD
	};

	template<typename InputIterator>
	Result parse(HttpRequest req, InputIterator begin, InputIterator end)
	{
		while (begin != end)
		{
			Result r = consume(req, *begin++);
			if (r == GOOD || r == BAD)
				return r;	
		}
		//todo validate components like max_uri_length??
		return INDETERMINATE;
	}

};

}//ws
}//servers
}//nbdl

#endif
