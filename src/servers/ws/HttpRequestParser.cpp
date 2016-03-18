//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include<cctype> //std::tolower
#include "HttpRequestParser.hpp"

namespace ws = nbdl::servers::ws;
using Result = ws::http_request_parser::result;

static const char CR = '\r';
static const char LF = '\n';

Result ws::http_request_parser::consume(char c)
{
	switch(state_)
	{
		case METHOD_GET_1:
			return require_char(c, 'G', METHOD_GET_2);
			break;
		
		case METHOD_GET_2:
			return require_char(c, 'E', METHOD_GET_3);
			break;
		
		case METHOD_GET_3:
			return require_char(c, 'T', METHOD_GET_SP);
			break;
		
		case METHOD_GET_SP:
			return require_char(c, ' ', URI);
			break;
		
		case URI:
			if (c == ' ')
				state_ = HTTP_1;
			else if (!is_ctl(c))
				request.uri.push_back(c);
			else
				return BAD_REQUEST;
			return INDETERMINATE;
			break;
		
		case HTTP_1:
			return require_char(c, 'H', HTTP_2);
			break;
		
		case HTTP_2:
			return require_char(c, 'T', HTTP_3);
			break;
		
		case HTTP_3:
			return require_char(c, 'T', HTTP_4);
			break;
		
		case HTTP_4:
			return require_char(c, 'P', HTTP_SLASH);
			break;
		
		case HTTP_SLASH:
			return require_char(c, '/', HTTP_VERSION_MAJOR);
			break;
		
		case HTTP_VERSION_MAJOR:
			return require_char(c, '1', HTTP_VERSION_POINT);
			break;
		
		case HTTP_VERSION_POINT:
			return require_char(c, '.', HTTP_VERSION_MINOR);
			break;
		
		case HTTP_VERSION_MINOR:
			return require_char(c, '1', REQUEST_LINE_CR);
			break;
		
		case REQUEST_LINE_CR:
			return require_char(c, CR, REQUEST_LINE_LF);
			break;
		
		case REQUEST_LINE_LF:
			return require_char(c, LF, HEADER_START);
			break;
		
		case HEADER_START:
			if (c == CR)
			{
				state_ = FINAL_LF;
				return INDETERMINATE;
			}
			state_ = HEADER_NAME;
			return process_header_name_char(c);
			break;
		
		case HEADER_NAME:
			if (c == ':')
			{
				state_ = HEADER_VALUE_START;
				return INDETERMINATE;
			}
			return process_header_name_char(c);
			break;
		
		case HEADER_VALUE_START:
			if (is_lws(c))
				return INDETERMINATE;
			state_ = HEADER_VALUE;
			return process_header_value_char(c);
			break;
		
		case HEADER_VALUE:
			if (c == CR)
			{
				header_value_complete();
				state_ = HEADER_LF;
				return INDETERMINATE;
			}
			return process_header_value_char(c);
			break;
		
		case HEADER_LF:
			return require_char(c, LF, HEADER_START);
			break;
		
		case FINAL_LF:
			if (c == LF)
			{
				state_ = AFTER_FINAL_LF;
				return GOOD;
			}
			else
				return BAD_REQUEST;
			break;
		
		case AFTER_FINAL_LF:
			return BAD_REQUEST;
			break;
	}
	return BAD_REQUEST;
}

Result ws::http_request_parser::require_char(char l, char r, state next)
{
	if (l != r)
		return BAD_REQUEST;
	state_ = next;
	return INDETERMINATE;
}

//don't read or determine state_ in these functions

bool ws::http_request_parser::is_lws(char c)
{
	return (c == ' ' || c == '\t');
}

bool ws::http_request_parser::is_ctl(char c)
{
	return (c <= 31 || c == 127);
}

bool ws::http_request_parser::is_ascii_non_ctl(char c)
{
	return (c > 31 && c < 127);
}

bool ws::http_request_parser::is_special_char(char c)
{
	switch (c)
	{
		case '(': case ')': case '<': case '>': case '@':
		case ',': case ';': case ':': case '\\': case '"':
		case '/': case '[': case ']': case '?': case '=':
		case '{': case '}': case ' ': case '\t':
			return true;
		default: 
			return false;
	}
}

bool ws::http_request_parser::is_valid_header_name_char(char c)
{
	return (!is_special_char(c) && is_ascii_non_ctl(c));		
}

bool ws::http_request_parser::is_valid_header_value_char(char c)
{
	return (!is_ctl(c));
}

Result ws::http_request_parser::process_header_name_char(char c)
{
	if (is_valid_header_name_char(c))
	{
		current_header_name.push_back(std::tolower(c));
		return INDETERMINATE;
	}
	return BAD_REQUEST;
}

Result ws::http_request_parser::process_header_value_char(char c)
{
	if (is_valid_header_value_char(c))
	{
		//todo convert to lowercase
		current_header_value.push_back(c);
		return INDETERMINATE;
	}
	return BAD_REQUEST;
}

void ws::http_request_parser::header_value_complete()
{
	if (current_header_name == "upgrade")
		request.upgrade_header = current_header_value;
	else if (current_header_name == "origin")
		request.origin_header = current_header_value;
	else if (current_header_name == "connection")
		request.connection_header = current_header_value;
	else if (current_header_name == "sec-websocket-key")
		request.sec_websocket_key_header = current_header_value;
	else if (current_header_name == "sec-websocket-version")
		request.sec_websocket_version_header = current_header_value;
	else if (current_header_name == "cookies")
		request.cookies_header = current_header_value;

	current_header_name.clear();
	current_header_value.clear();
}
