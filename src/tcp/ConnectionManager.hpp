//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_TCP_CONNECTION_MANAGER_HPP
#define NBDL_TCP_CONNECTION_MANAGER_HPP

#include "tcp/Connection.hpp"

namespace nbdl {
namespace tcp {

template<typename ConnectionHandler>
class ConnectionManager
{
	using Socket = typename Connection::Socket;
	using ConnectionPtr = std::shared_ptr<Connection>;
	std::set<std::shared_ptr<Connection>> container;

	public:

	template<typename... Args>
	ConnectionManager(Args... args) :
		handler(args...)
	{}

	void start(Socket socket)
	{
		container.insert(
			makeConnection(
				std::move(socket),
				ConnectionHandler(),
				[&](ConnectionPtr& ptr) {
					container.erase(ptr);
				}
			)
		);
	}

	void stopAll()
	{
		for (auto c: container)
		{
			c->stop();
		}
		container.clear();
	}
}

}//tcp
}//nbdl

#endif
