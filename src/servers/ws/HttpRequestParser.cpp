//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include<cctype> //std::tolower
#include "HttpRequestParser.hpp"

namespace ws = nbdl::servers::ws;
using Result = ws::HttpRequestParser::Result;

static const char CR = '\r';
static const char LF = '\n';

Result ws::HttpRequestParser::consume(char c)
{
	switch(state_)
	{
		case METHOD_GET_1:
			return requireChar(c, 'G', METHOD_GET_2);
			break;
		
		case METHOD_GET_2:
			return requireChar(c, 'E', METHOD_GET_3);
			break;
		
		case METHOD_GET_3:
			return requireChar(c, 'T', METHOD_GET_SP);
			break;
		
		case METHOD_GET_SP:
			return requireChar(c, ' ', URI);
			break;
		
		case URI:
			if (c == ' ')
				state_ = HTTP_1;
			else if (!isCtl(c))
				request.uri.push_back(c);
			else
				return BAD_REQUEST;
			return INDETERMINATE;
			break;
		
		case HTTP_1:
			return requireChar(c, 'H', HTTP_2);
			break;
		
		case HTTP_2:
			return requireChar(c, 'T', HTTP_3);
			break;
		
		case HTTP_3:
			return requireChar(c, 'T', HTTP_4);
			break;
		
		case HTTP_4:
			return requireChar(c, 'P', HTTP_SLASH);
			break;
		
		case HTTP_SLASH:
			return requireChar(c, '/', HTTP_VERSION_MAJOR);
			break;
		
		case HTTP_VERSION_MAJOR:
			return requireChar(c, '1', HTTP_VERSION_POINT);
			break;
		
		case HTTP_VERSION_POINT:
			return requireChar(c, '.', HTTP_VERSION_MINOR);
			break;
		
		case HTTP_VERSION_MINOR:
			return requireChar(c, '1', REQUEST_LINE_CR);
			break;
		
		case REQUEST_LINE_CR:
			return requireChar(c, CR, REQUEST_LINE_LF);
			break;
		
		case REQUEST_LINE_LF:
			return requireChar(c, LF, HEADER_START);
			break;
		
		case HEADER_START:
			if (c == CR)
			{
				state_ = FINAL_LF;
				return INDETERMINATE;
			}
			state_ = HEADER_NAME;
			return processHeaderNameChar(c);
			break;
		
		case HEADER_NAME:
			if (c == ':')
			{
				state_ = HEADER_VALUE_START;
				return INDETERMINATE;
			}
			return processHeaderNameChar(c);
			break;
		
		case HEADER_VALUE_START:
			if (isLws(c))
				return INDETERMINATE;
			state_ = HEADER_VALUE;
			return processHeaderValueChar(c);
			break;
		
		case HEADER_VALUE:
			if (c == CR)
			{
				headerValueComplete();
				state_ = HEADER_LF;
				return INDETERMINATE;
			}
			return processHeaderValueChar(c);
			break;
		
		case HEADER_LF:
			return requireChar(c, LF, HEADER_START);
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

Result ws::HttpRequestParser::requireChar(char l, char r, State next)
{
	if (l != r)
		return BAD_REQUEST;
	state_ = next;
	return INDETERMINATE;
}

//don't read or determine state_ in these functions

bool ws::HttpRequestParser::isLws(char c)
{
	return (c == ' ' || c == '\t');
}

bool ws::HttpRequestParser::isCtl(char c)
{
	return (c <= 31 || c == 127);
}

bool ws::HttpRequestParser::isAsciiNonCtl(char c)
{
	return (c > 31 && c < 127);
}

bool ws::HttpRequestParser::isSpecialChar(char c)
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

bool ws::HttpRequestParser::isValidHeaderNameChar(char c)
{
	return (!isSpecialChar(c) && isAsciiNonCtl(c));		
}

bool ws::HttpRequestParser::isValidHeaderValueChar(char c)
{
	return (!isCtl(c));
}

Result ws::HttpRequestParser::processHeaderNameChar(char c)
{
	if (isValidHeaderNameChar(c))
	{
		current_header_name.push_back(std::tolower(c));
		return INDETERMINATE;
	}
	return BAD_REQUEST;
}

Result ws::HttpRequestParser::processHeaderValueChar(char c)
{
	if (isValidHeaderValueChar(c))
	{
		//todo convert to lowercase
		current_header_value.push_back(c);
		return INDETERMINATE;
	}
	return BAD_REQUEST;
}

void ws::HttpRequestParser::headerValueComplete()
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
