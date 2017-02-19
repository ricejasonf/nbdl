//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//


#include <iostream>
#include <nbdl.hpp>
#include <nbdl/binder/jsoncpp.hpp>

auto tap = [](auto fn)
{
  return nbdl::promise([fn_ = std::move(fn)](auto&& resolve, auto&& ...args)
  {
    fn_(args...);
    resolve(std::forward<decltype(args)>(args)...);
  });
};

#include "accept.hpp"
#include "connect.hpp"
#include "hana_printable.hpp"
#include "receive_messages_until.hpp"
#include "read_raw_message.hpp"
#include "write_raw_message.hpp"

#include <asio.hpp>
#include <boost/hana.hpp>
#include <iostream>
#include <nbdl.hpp>
#include <nbdl/binder/jsoncpp.hpp>

namespace hana = boost::hana;
namespace hanax = boost::hana::experimental;
using namespace hana::literals;

struct person_t
{
  std::string name_first;
  std::string name_last;
};

struct user_t
{
  std::string username;
  person_t person;
};

// setup compile-time reflection
namespace nbdl
{
  NBDL_ENTITY(person_t
  , name_first
  , name_last
  );
  NBDL_ENTITY(user_t
  , username
  , person
  );
}

// type that will signal the server to
// close the connection
struct terminate { };

// The server will print the raw json it
// receives followed by this.
auto print = hana::overload_linearly(
  [](user_t const& user)
  {
    std::cout << "Received user with username: " << user.username << "\n\n";
  }
, [](person_t const& person)
  {
    std::cout << "Received person with first name: " << person.name_first << "\n\n";
  }
, [](terminate) 
  {
    std::cout << "Received terminate object. Good bye!\n\n";
  }
, [](auto const& x)
    -> std::enable_if_t<hanax::Printable<decltype(x)>::value>
  {
    std::cout << "Received Hana Printable: " << hanax::print(x) << "\n\n";
  }
);

int main()
{
  using namespace std::string_literals;

  // heterogeneous list of test messages
  // All of these will be converted to json
  // and back.
  auto messages = hana::make_tuple(
    person_t{"John", "Smith"}
  , user_t{"@JasonRice_", person_t{"Jason", "Rice"}}
  , hana::make_tuple(
      "This"s , "is a tuple with"s , 6, "elements"s
    , "I'm a compile time string!"_s
    , "Last"s
    )
  , hana::make_map(
      hana::make_pair("key_1"_s, "value1"s)
    , hana::make_pair("key_2"_s, "value2"s)
    , hana::make_pair("key_3"_s, hana::just(5))
    , hana::make_pair("key_4"_s, hana::nothing)
    , hana::make_pair("key_5"_s, hana::make_tuple(1, 2, 3, 4, "tuple"s))
    )
  , terminate{}
  );

  // message_variant will store
  // type information when serialized.
  using message_variant = typename decltype(hana::unpack(
    hana::transform(messages, hana::typeid_)
  , hana::template_<nbdl::variant>
  ))::type;

  // asio stuff
  asio::io_service io;
  asio::ip::tcp::socket client_socket(io);

  // creates a promise that will send a message
  auto send_message = [&](auto&& message) {
    return nbdl::pipe(
      hana::always(message_variant(std::forward<decltype(message)>(message)))
    , nbdl::binder::jsoncpp::to_string
    , example::write_raw_message(client_socket)
    );
  };

  // promise to connect and send all messages sequentially
  auto send_messages = nbdl::pipe(
    example::connect(client_socket, example::port{1234})
  , tap([](){ std::cout << "connected to server\n"; })
  , hana::unpack(std::move(messages), nbdl::pipe ^hana::on^ send_message)
  , nbdl::catch_([](example::attempts) { std::cout << "Client failed with too many attempts.\n"; })
  , nbdl::catch_([](auto error) { std::cout << "CLIENT ERROR: " << error.message() <<'\n'; })
  );

  // promise to accept a connection and print messages
  auto receive_messages = nbdl::pipe(
    example::accept(io, example::port({1234}))
  , tap([](auto&&...){ std::cout << "client connection accepted\n"; })
  , example::receive_messages_until(
      hana::type_c<message_variant>
    , hana::type_c<terminate>
    , print
    )
  , nbdl::catch_([](auto error) { std::cout << "SERVER ERROR: " << error.message() <<'\n'; })
  );

  receive_messages();
  send_messages();

  // When the messages are sent and the server gets
  // the terminate message io.run() will run out of events,
  // and stop blocking effectively ending the process.
  io.run();

  // fin
}
