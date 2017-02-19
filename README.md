# Nbdl

>Network Based Data Layer
>
>Generic C++ Library for Managing Application State Across Network 

## Overview

This project is work in progress.

Basic useful feature list:

* Context object for managing state, network connection, and message dispatching
* Traits/Concepts for wrapping server/client implementations and stores
* Traits/Concepts for serialization of Boost.Hana and user-defined data types
* Pipeable promise interface for chaining events
* A variant type optimized for use on a single thread, with a safe interface,
  and most importantly it plays well with the serialization facilities
  which is used for dispatching messages and representing state.

### Building a Context Type

```cpp
  // Excerpt from example/tiles/game.hpp (wip)

  template <typename ProviderTag, typename ConsumerTag>
  struct context
  {
    static constexpr auto make_def()
    {
      using namespace nbdl_def;
      namespace hana = boost::hana;
      return
        Context(
          Entities(
            Entity(Type(hana::type_c<game>)),
            Entity(Type(hana::type_c<game_move>))
          ),
          Provider(
            Type(ProviderTag{}),
            AccessPoint(
              EntityName(hana::type_c<game>),
              Store(hana::type_c<game_store>),
              Actions(),
              AccessPoint(
                EntityName(hana::type_c<game_move>),
                Actions(Create())
              )
            )
          ),
          Consumer(Type(ConsumerTag{}))
        );
      }
  };

```

This generates a type that owns a provider, consumer, and stores. It also generates variants for both upstream and downstream messages for all of the entity actions defined.

### Event Chaining with `nbdl::pipe`

```cpp
  // Excerpt from example/pipes/main.cpp

  // creates a promise that will send a message
  auto send_message = [&](auto&& message) {
    return nbdl::pipe(
      hana::always(my_message_variant(std::forward<decltype(message)>(message)))
    , nbdl::binder::jsoncpp::to_string
    , example::write_raw_message(client_socket)
    );
  };

  // promise to connect and send messages sequentially
  auto send_messages = nbdl::pipe(
    example::connect(client_socket, example::port{1234})
  , tap([](){ std::cout << "connected to server\n"; })
  , hana::unpack(messages, nbdl::pipe ^hana::on^ send_message)
  , nbdl::catch_([](example::attempts) { std::cout << "Client failed with too many attempts.\n"; })
  , nbdl::catch_([](auto error) { std::cout << "CLIENT ERROR: " << error.message() <<'\n'; })
  );
```

### Dispatching serialized messages with `nbdl::variant`

```cpp
  // Excerpt from example/pipes/main.cpp

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

  message_variant input_msg = person_t{"John", "Smith"};
  std::string msg_str = nbdl::binder::jsoncpp::to_string(msg);

  // The following could be on the other end of a socket.

  message_variant output_msg;

  nbdl::binder::jsoncpp::from_string(msg_str, output_msg);

  // overloads linearly
  output_msg.match(
    [](person_t const& p)
    {
      // Here we can do whatever logic we want for when we receive a "person".
      std::cout << "Received person with first name: " << person.name_first;
    },
    [](auto&&)
    { 
      std::cout << "Received some other type of message.";
    }
  );
```

Here a type `message_variant` is created with the types in the tuple `messages`.
An instance of `message_variant` can be created with any of the types and then
serialized to json and unserialized on the other end of a tcp socket or whatever.

### Playing with the Examples

Currently Nbdl's dependencies are not trivial to deal with, but if you have Docker installed it is easy to get started. As a convenience, a GNU Makefile in the root directory simplifies the use of docker further.


```
make image && make develop
```

Simply run these to download the image and start the development container. This creates a build directory in the container with cmake initialized for you ready to go with a recent version of Clang. Then at the prompt you can build various targets using `make`.

```
root@f2b146e7bad9:/opt/build# make run.example.pipes
```
