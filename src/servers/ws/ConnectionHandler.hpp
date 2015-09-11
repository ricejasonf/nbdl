//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_SERVERS_WS_CONNECTION_HANDLER_HPP
#define NBDL_SERVERS_WS_CONNECTION_HANDLER_HPP

#include<vector>
#include "servers/ws/Parser.hpp"

namespace nbdl {
namespace servers {
namespace ws {

template<typename Context>
class ConnectionHandler
{
	Parser parser;

	public:

	template<typename InputIterator>
	void readSome(InputIterator begin, InputIterator end)
	{
		while (begin != end)
		{
			switch(parser.parse(*begin))
			{
				case Parser::Indeterminate:
					//keep going
					++begin;
					break;
				case Parser::ParseMsg:
					//maybe this should be internal to the parser yeah?
					//use the concrete msg parser
					msg_buf.push_back(*begin);
					if (parser.parseMsg(*begin))
					{
						//something with parser.getMsgBuffer()
						//complete ship the msg
						//uhhh hello?
					}
					++begin;
					break;
				case Parser::UpgradeRequired:
					//websocket upgrade was not specified
					//splode
					//upgradeRequired()
					return;
					break;
				case Parser::BadRequest:
					//splode
					//badRequest();
					return;
					break;
				//todo incompatible api version
			}
		}
	}

};

}//ws
}//servers
}//nbdl

#endif
