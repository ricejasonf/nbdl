//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_TCP_SERVER_HPP
#define NBDL_TCP_SERVER_HPP

#include<csignal>
#include<asio.hpp>
#include<string>
#include<tcp/ConnectionManager.hpp>

namespace nbdl {
namespace tcp {

template<typename RequestHandler>
class Server
{
	//at some point they changed the name
	//asio::io_context io_ctx;
	asio::io_service io_ctx; 
	asio::signal_set signals;
	asio::ip::tcp::acceptor acceptor;
	ConnectionManager<RequestHandler> connections;

	public:

	Server(const Server&) = delete;
	Server& operator= (const Server&) = delete;

	template<typename... Args>
	explicit Server(const std::string& address, const std::string& port
			Args... args) :
		io_ctx(),
		signals(io_ctx),
		connections(args...)
	{
		signals.add(SIGINT);
		signals.add(SIGTERM);
#ifdef SIGQUIT
		signals.add(SIGQUIT);
#endif//SIGQUIT
		awaitStop();
		
		asio::ip::tcp::resolver resolver(io_ctx);
		asio::ip::tcp::endpoint endpoint =
			*resolver.resolve(address, port).begin();
		acceptor.open(endpoint.protocol());
		acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
		acceptor.bind(endpoint);
		acceptor.listen();

		awaitAccept();
	}

	void start()
	{
		//blocks until async operations are complete
		io_ctx.run();
	}

	void accept()
	{
		using Socket = asio::ip::tcp::socket;
		acceptor.async_accept([this](std::error_code error, Socket socket) {
			if (!acceptor.is_open())
			{
				return;
			}
			if (!ec)
			{
				connections.start(std::move(socket));
			}
			accept();
		})
	}
	
	void awaitStop()
	{
		signals.async_wait([this](std::error_code, int) {
			acceptor.close();
			connections.stopAll();
		});
	}
};

}//tcp
}//nbdl

#endif
