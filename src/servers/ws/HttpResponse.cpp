#include<cstring>
#include<openssl/sha.h>
#include "HttpResponse.hpp"
#include "../../openssl/Base64Encode.hpp"

namespace ws = nbdl::servers::ws;
using Base64Encode = nbdl::openssl::Base64Encode;

void ws::http_response::generate_accept_token(const std::string& key)
{
	const char* guid = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	constexpr int guid_len = 36;
	SHA_CTX ctx;
	unsigned char digest[SHA_DIGEST_LENGTH];

	SHA1_Init(&ctx);
	SHA1_Update(&ctx, key.c_str(), key.size());
	SHA1_Update(&ctx, guid, guid_len);
	SHA1_Final(digest, &ctx);
	Base64Encode encoding(reinterpret_cast<const char*>(digest), SHA_DIGEST_LENGTH);
	encoding.appendTo(response);
}
