//
// Copyright Jason Rice 2014-2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/detail/beast_sha1.hpp>
#include <nbdl/util/base64_encode.hpp>

#include <array>
#include "HttpResponse.hpp"

namespace ws = nbdl::servers::ws;
namespace sha1 = nbdl::detail::beast_sha1;

void ws::http_response::generate_accept_token(std::string const& key)
{
  using nbdl::util::base64_encode;

	const char* guid = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	constexpr int guid_length = 36;
  std::array<unsigned char, 20> digest{};
  sha1::sha1_context ctx{};

  sha1::init(ctx);
  sha1::update(ctx, key.data(), key.length());
  sha1::update(ctx, guid, guid_length);
  sha1::finish(ctx, digest.data());

  response.append(base64_encode(digest));
}
