//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_WEBSOCKET_DETAIL_MESSAGE_SENDER_HPP
#define NBDL_WEBSOCKET_DETAIL_MESSAGE_SENDER_HPP

#include <nbdl/promise.hpp>

#include <algorithm>
#include <asio.hpp>
#include <boost/hana/integral_constant.hpp>
#include <iterator>

namespace nbdl::websocket::detail
{
  using asio::ip::tcp;
  namespace hana = boost::hana;
  using namespace std::string_view_literals;
  using std::string_view;

  enum class opcode_kind
    : unsigned char
  {
    continuation  = 0x0
  , text          = 0x1
  , binary        = 0x2
  , close         = 0x8
  , ping          = 0x9
  , pong          = 0xA
  };

  template <typename Payload>
  struct message_data;

  template <typename Payload, typename MaxFrameSize = hana::size_t<125>>
  struct message_sender
  {
    static constexpr std::size_t max_frame_size = MaxFrameSize::value;

    message_sender(tcp::socket& s)
      : socket(s)
      , data()
    { }

    template <typename Handler>
    void send_frame_impl(asio::const_buffer, Handler&&);

    template <typename Resolver, typename Begin, typename End>
    void send_message_impl(Resolver&, Begin, End);

    void set_opcode(unsigned char x) { data.header_byte = x; }

    Payload& payload() { return data.payload; }

    tcp::socket& socket;
    detail::message_data<Payload> data;
  };

  template <typename Payload>
  struct message_data
  {
    void apply_fin_bit()
    {
      header_byte |= (1 << 7);
    }

    void apply_mask_bit(std::size_t)
    {
      // Masks are required for client-to-server frames.
      // Having the server require it is silly because
      // anyone can provide any mask they want to.
      // This implementation relaxes that requirement.
    }

    asio::const_buffer encoded_length_buffer(std::size_t);

    unsigned char                  header_byte;
    std::array<unsigned char, 9>   length_data;
    Payload                        payload;
  };

  template <typename Payload>
  asio::const_buffer message_data<Payload>::encoded_length_buffer(std::size_t length)
  {
    if (length < 126)
    {
      length_data[0] = length;
      return asio::const_buffer(static_cast<void const*>(length_data.data()), 1);
    }
    else if (length < 65536)
    {
      // 16 bit + 1 byte
      length_data[0] = 126;
      length_data[1] = length >> 8;
      length_data[2] = length;
      return asio::const_buffer(static_cast<void const*>(length_data.data()), 3);
    }
    else
    {
      // 64 bit + 1 byte
      length_data[0] = 127;
      length_data[1] = length >> 56;
      length_data[2] = length >> 48;
      length_data[3] = length >> 40;
      length_data[4] = length >> 32;
      length_data[5] = length >> 24;
      length_data[6] = length >> 16;
      length_data[7] = length >>  8;
      length_data[8] = length;
      return asio::const_buffer(static_cast<void const*>(length_data.data()), 9);
    }
  };

  template <typename Payload, typename MaxFrameSize>
  template <typename Handler>
  void message_sender<Payload, MaxFrameSize>::
  send_frame_impl( asio::const_buffer frame_payload_buffer
                 , Handler&& handler
                 )
  {
    asio::async_write(
      socket
    , std::array<asio::const_buffer, 3>{{
        asio::buffer(static_cast<void const*>(&data.header_byte), 1)
      , data.encoded_length_buffer(frame_payload_buffer.size())
      , frame_payload_buffer
      }}
    , std::forward<Handler>(handler)
    );
  }

  template <typename Payload, typename MaxFrameSize>
  template <typename Resolver, typename PayloadBegin, typename PayloadEnd>
  void message_sender<Payload, MaxFrameSize>::
  send_message_impl(Resolver& resolver, PayloadBegin begin, PayloadEnd end)
  {
    auto frag_end = std::next(begin, MaxFrameSize::value);

    if (frag_end > end)
    {
      frag_end = end;
      data.apply_fin_bit();
    }

    auto frame_payload_buffer = asio::buffer(
      static_cast<void const*>(&(*begin))
    , std::distance(begin, frag_end)
    );

    send_frame_impl(
      frame_payload_buffer
    , [new_begin = frag_end, end, &resolver, this](auto error_code, std::size_t)
      {
        if (error_code)
        {
          resolver.reject(error_code);
        }
        else if (new_begin == end)
        {
          resolver.resolve(*this);
        }
        else
        {
          set_opcode(static_cast<unsigned char>(opcode_kind::continuation));
          // generate_mask();
          send_message_impl(resolver, new_begin, end);
        }
      }
    );
  }

  template <unsigned char opcode>
  struct send_impl_fn
  {
    template <typename Resolver, typename MessageSender>
    void operator()(Resolver& resolver, MessageSender& sender) const
    {
      sender.set_opcode(opcode);
      sender.send_message_impl(
        resolver
      , sender.data.payload.begin()
      , sender.data.payload.end()
      );
    }
  };

  template <unsigned char opcode>
  constexpr auto send_impl = send_impl_fn<opcode>{};

  constexpr auto send_text   = detail::send_impl< static_cast<unsigned char>(opcode_kind::text)  >;
  constexpr auto send_binary = detail::send_impl< static_cast<unsigned char>(opcode_kind::binary)>;
  constexpr auto send_close  = detail::send_impl< static_cast<unsigned char>(opcode_kind::close) >;
  constexpr auto send_ping   = detail::send_impl< static_cast<unsigned char>(opcode_kind::ping)  >;
  constexpr auto send_pong   = detail::send_impl< static_cast<unsigned char>(opcode_kind::pong)  >;
}

#endif
