//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_SERVERS_WS_PARSER_HPP
#define NBDL_SERVERS_WS_PARSER_HPP

#include<string>

namespace nbdl {
namespace servers {
namespace ws {

struct HttpRequest
{
	std::string uri;
	std::string upgrade_header;
	std::string origin_header;
	std::string connection_header;
	std::string sec_websocket_key_header;
	std::string sec_websocket_version_header;
	std::string cookies_header;
};


class HttpRequestParser
{
	public:

	enum Result {
		INDETERMINATE,
		GOOD,
		BAD_REQUEST
	};

	private:

	enum State {
		METHOD_GET_1,
		METHOD_GET_2,
		METHOD_GET_3,
		METHOD_GET_SP,
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
		AFTER_FINAL_LF //??
	} state_;

	Result consume(char c);
	Result requireChar(char l, char r, State next);
	bool isLws(char);
	bool isCtl(char);
	bool isAsciiNonCtl(char);
	bool isSpecialChar(char);
	bool isValidHeaderNameChar(char);
	bool isValidHeaderValueChar(char);
	Result processHeaderValueChar(char);
	Result processHeaderNameChar(char c);
	void headerValueComplete();

	//headers
	std::string current_header_name;
	std::string current_header_value;
	HttpRequest request;

	public:

	HttpRequestParser() :
		state_(METHOD_GET_1)
	{
		current_header_name.reserve(50);
		current_header_value.reserve(50);
	}


	template<typename InputIterator>
	Result parse(InputIterator begin, InputIterator end)
	{
		while (begin != end)
		{
			Result r = consume(*begin++);
			if (r == GOOD || r == BAD_REQUEST)
				return r;	
		}
		//todo validate components like max_uri_length??
		return INDETERMINATE;
	}

	HttpRequest getRequest()
	{
		return request;
	}

};

}//ws
}//servers
}//nbdl

#endif
