//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_TCP_CONNECTION_HPP
#define NBDL_TCP_CONNECTION_HPP

#include<memory>
#include<asio.hpp>

namespace nbdl {
namespace tcp {

template<Socket, ConnectionHandler, RemoveFn>
Connection<Socket, ConnectionHandler, RemoveRn> 
makeConnection(Socket socket, ConnectionHandler handler, RemoveFn removeFn)
{
	return std::make_shared<Connection<Socket, ConnectionHandler, RemoveRn>>(
		socket,
		handler,
		removeFn
	);
}

template<Socket, ConnectionHandler, RemoveFn>
class Connection : public 
	std::enable_shared_from_this<Connection<
		Socket, ConnectionHandler, RemoveFn>>
{
	Socket socket;
	ConnectionHandler handler;
	RemoveFn removeMe; //removes self from manager

	std::array<char, 8192> read_buffer;

	void stopMe()
	{
		stop();
		removeMe();
	}

	public:

	Connection(Socket s, ConnectionHandler h, RemoveFn d) :
		socket(s),
		ConnectionHandler(h),
		RemoveFn(d)
	{}

	void start()
	{
		read();	
	}

	void stop()
	{
		socket.close();
	}

	void read()
	{
		auto self(shared_from_this());
		socket.async_read_some(asio::buffer(read_buffer),
			[this, self](std::error_code ec, std::size_t bytes_transferred)
			{
				if (!ec)
				{
					handler.readSome(read_buffer.data(), bytes_transferred);
					read();
				}
				else
				{
					stopMe();
				}
			}
		);
	}
	//todo enable nbdl context to write revisions as buffers to connection
	//write to connection handler that is
};

}//tcp
}//nbdl

#endif
