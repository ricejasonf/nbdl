//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_OPENSSL_BASE64_ENCODE_HPP
#define NBDL_OPENSSL_BASE64_ENCODE_HPP

#include<string>
#include<openssl/bio.h>

namespace nbdl {
namespace openssl {

class Base64Encode
{
	BIO *mem, *b64;

	public:
	
	Base64Encode(const char* input, int input_len);
	~Base64Encode();
	Base64Encode(const Base64Encode&) = delete;

	void appendTo(std::string& str);
};

}//openssl
}//nbdl

#endif
