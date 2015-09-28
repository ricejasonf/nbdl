//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "Base64Encode.hpp"
#include<openssl/bio.h>
#include<openssl/evp.h>

namespace ssl = nbdl::openssl; 

ssl::Base64Encode::Base64Encode(const char* input, int input_length)
{
	b64 = BIO_new(BIO_f_base64());
	mem = BIO_new(BIO_s_mem());
	BIO_push(b64, mem);

	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
	BIO_write(b64, input, input_length);
	BIO_flush(b64);
}

ssl::Base64Encode::~Base64Encode()
{
	BIO_free_all(b64);
}

void ssl::Base64Encode::appendTo(std::string& str)
{
	unsigned char* output;
	long len = BIO_get_mem_data(mem, &output);
	str.append((char*) output, (int) len);
}
