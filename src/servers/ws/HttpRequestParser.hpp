//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_SERVERS_WS_HTTP_REQUEST_PARSER_HPP
#define NBDL_SERVERS_WS_HTTP_REQUEST_PARSER_HPP

#include<string>

namespace nbdl {
namespace servers {
namespace ws {

struct http_request
{
	std::string uri;
	std::string upgrade_header;
	std::string origin_header;
	std::string connection_header;
	std::string sec_websocket_key_header;
	std::string sec_websocket_version_header;
	std::string cookies_header;
};


class http_request_parser
{
	public:

	enum result {
		INDETERMINATE,
		GOOD,
		BAD_REQUEST
	};

	private:

	enum state {
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

	result consume(char c);
	result require_char(char l, char r, state next);
	bool is_lws(char);
	bool is_ctl(char);
	bool is_ascii_non_ctl(char);
	bool is_special_char(char);
	bool is_valid_header_name_char(char);
	bool is_valid_header_value_char(char);
	result process_header_value_char(char);
	result process_header_name_char(char c);
	void header_value_complete();

	//headers
	std::string current_header_name;
	std::string current_header_value;
	http_request request;

	public:

	http_request_parser() :
		state_(METHOD_GET_1)
	{
		current_header_name.reserve(50);
		current_header_value.reserve(50);
	}


	template<typename InputIterator>
	result parse(InputIterator begin, InputIterator end)
	{
		while (begin != end)
		{
			result r = consume(*begin++);
			if (r == GOOD || r == BAD_REQUEST)
				return r;	
		}
		//todo validate components like max_uri_length??
		return INDETERMINATE;
	}

	http_request get_request()
	{
		return request;
	}

};

}//ws
}//servers
}//nbdl

#endif
