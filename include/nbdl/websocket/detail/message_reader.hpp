//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_WEBSOCKET_DETAIL_MESSAGE_READER_HPP
#define NBDL_WEBSOCKET_DETAIL_MESSAGE_READER_HPP

#include <nbdl/promise.hpp>
#include <nbdl/run_async_loop.hpp>

#include <algorithm>
#include <asio.hpp>
#include <boost/hana/tuple.hpp>
#include <cstdint>

namespace nbdl::websocket::detail
{
  using asio::ip::tcp;
  namespace hana = boost::hana;

  namespace read_event
  {
    struct message_t { };
    struct close_t { };
    struct bad_input_t { };
    struct ping_t { };
    struct pong_t { };

    constexpr auto message      = hana::type_c<message_t>;
    constexpr auto close        = hana::type_c<close_t>;
    constexpr auto bad_input    = hana::type_c<bad_input_t>;
    constexpr auto ping         = hana::type_c<ping_t>;
    constexpr auto pong         = hana::type_c<pong_t>;
  }

  constexpr auto handle_next = [](auto& resolver, auto& frame_ctx)
  {
    return [&](auto error_code, std::size_t)
    {
      if (error_code)
      {
        resolver.reject(error_code);
      }
      else
      {
        resolver.resolve(frame_ctx);
      }
    };
  };

  template <typename Payload>
  struct frame_context_t
  {
    tcp::socket& socket;
    std::array<unsigned char, 2> buf_1;
    std::array<unsigned char, 8> extended_length_buf;
    std::array<unsigned char, 4> mask_key;
    Payload payload;

    frame_context_t(tcp::socket& s)
      : socket (s)
      , buf_1{}
    { }
  };

  template <typename FrameContext>
  inline bool has_mask_key(FrameContext const& frame_ctx)
  {
    return (frame_ctx.buf_1[1] >> 7) != 0;
  }


  template <typename FrameContext>
  inline bool has_fin_bit(FrameContext const& frame_ctx)
  {
    return (frame_ctx.buf_1[0] >> 7) != 0;
  }

  auto read_frame_header = nbdl::promise([](auto& resolver, auto& frame_ctx)
  {
    // TODO require continuation opcode
    // (not sure if it would matter)
    asio::async_read(
      frame_ctx.socket
    , asio::buffer(frame_ctx.buf_1, 2)
    , handle_next(resolver, frame_ctx)
    );
  });

  auto read_extended_length = nbdl::promise([](auto& resolver, auto& frame_ctx)
  {
    uint8_t length_code = frame_ctx.buf_1[1] & 0x7F; // Removes MASK bit

    if (length_code == 126)
    {
      asio::async_read(
        frame_ctx.socket
      , asio::buffer(frame_ctx.extended_length_buf, 2)
      , handle_next(resolver, frame_ctx)
      );
    }
    else if (length_code == 127)
    {
      asio::async_read(
        frame_ctx.socket
      , asio::buffer(frame_ctx.extended_length_buf, 8)
      , handle_next(resolver, frame_ctx)
      );
    }
    else
    {
      resolver.resolve(frame_ctx);
    }
  });

  auto read_masking_key = nbdl::promise([](auto& resolver, auto& frame_ctx)
  {
    unsigned char c = frame_ctx.buf_1[1];
    if ((c >> 7) != 0) // has mask
    {
      asio::async_read(
        frame_ctx.socket
      , asio::buffer(frame_ctx.mask_key, 4)
      , handle_next(resolver, frame_ctx)
      );
    }
    else
    {
      resolver.resolve(frame_ctx);
    }
  });

  template <typename FrameContext>
  inline uint64_t get_payload_length(FrameContext const& x)
  {
    uint64_t length = 0;
    
    uint8_t length_code = x.buf_1[1] & 0x7F; // Removes MASK bit

    if (length_code == 126)
    {
      // 16 bit
      for (int i = 0; i < 2; i++)
      {
        length |= (x.extended_length_buf[1 - i] << (8 * i));
      }
    }
    else if(length_code == 127)
    {
      // 64 bit
      for (int i = 0; i < 8; i++)
      {
        length |= (x.extended_length_buf[7 - i] << (8 * i));
      }
    }
    else
    {
      return length_code;
    }
    return length;
  }

  constexpr auto read_payload = [](auto& resolver, auto& frame_ctx)
  {
    const int length = get_payload_length(frame_ctx);
    // we are appending a fixed length of bytes
    // to a possibly existing payload
    auto& payload = frame_ctx.payload;
    std::size_t current_offset = payload.size();
    payload.resize(payload.size() + length);

    asio::async_read(
      frame_ctx.socket
    , asio::buffer(&payload[current_offset], length)
    , [&](auto error_code, std::size_t)
      {
        if (error_code)
        {
          resolver.reject(error_code);
        }
        else
        {
          if (has_mask_key(frame_ctx))
          {
            for (int i = 0; i < length; i++)
            {
              payload[i] ^= frame_ctx.mask_key[i % 4];
            }
          }
          resolver.resolve(read_event::message);
        }
      }
    );
  };

  // control frame 'close' can have a payload of <= 127 bytes
  constexpr auto read_close_payload = [](auto& resolver, auto& frame_ctx)
  {
    const int length = get_payload_length(frame_ctx);

    // destroy the existing payload
    auto& payload = frame_ctx.payload;
    payload.resize(length, 0);

    asio::async_read(
      frame_ctx.socket
    , asio::buffer(payload)
    , [&](auto error_code, std::size_t)
      {
        if (error_code)
        {
          resolver.reject(error_code);
        }
        else
        {
          if (has_mask_key(frame_ctx))
          {
            for (int i = 0; i < length; i++)
            {
              payload[i] ^= frame_ctx.mask_key[i % 4];
            }
          }
          resolver.reject(read_event::close);
        }
      }
    );
  };

  // This kind of dispatching can be done only because
  // all of the branches are stateless.
  auto opcode_dispatch = nbdl::promise([](auto& resolver, auto& frame_ctx)
  {
    switch (frame_ctx.buf_1[0] & 0x0F) // Removes FIN bit
    {
      case 0x0: // DATA
      case 0x1:
      case 0x2:
        read_payload(resolver, frame_ctx);
        break;
      case 0x8: // CLOSE
        read_close_payload(resolver, frame_ctx);
        break;
      case 0x9: // PING
        resolver.resolve(read_event::ping);
        break;
      case 0xA: // PONG
        resolver.resolve(read_event::pong);
        break;
      default:
        resolver.reject(read_event::bad_input);
    }
  });

  template <typename Payload, typename EventHandler>
  struct message_reader
  {
    message_reader(tcp::socket& socket, EventHandler h)
      : frame_ctx(socket)
      , handler(h)
    { }

    message_reader(message_reader const&) = delete;

    template <typename Shared>
    void keep_reading(Shared shared)
    {
      nbdl::run_async_loop(
        hana::make_tuple(
          read_frame_header
        , read_extended_length
        , read_masking_key
        , opcode_dispatch
        , [this](auto event_type) -> frame_context_t<Payload>&
          {
            if (event_type == read_event::message)
            {
              if (has_fin_bit(frame_ctx))
              {
                handler[event_type](event_type, frame_ctx);
                frame_ctx.payload.clear();
              }
            }
            else
            {
              handler[event_type](event_type, frame_ctx);
            }
            return frame_ctx;
          }
        , nbdl::catch_([this, shared = std::move(shared)](auto event)
          {
            handler[hana::typeid_(event)](event, frame_ctx);
          })
        )
      , frame_ctx
      );
    }

  private:

    frame_context_t<Payload> frame_ctx;
    EventHandler handler;
  };

  constexpr auto make_message_reader = [](auto payload_type, tcp::socket& socket, auto&& handler)
  {
    using Payload = typename decltype(payload_type)::type;
    using MessageReader = message_reader<Payload, std::decay_t<decltype(handler)>>;
    return MessageReader(socket, std::forward<decltype(handler)>(handler));
  };
}

#endif
