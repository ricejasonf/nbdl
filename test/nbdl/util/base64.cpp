//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/util/base64_encode.hpp>
#include <nbdl/util/base64_decode.hpp>

#include <boost/hana.hpp>
#include <catch.hpp>
#include <string>
#include <vector>

template <typename Buffer>
std::vector<unsigned char> to_vec(Buffer const& b)
{
  return std::vector<unsigned char>(b.begin(), b.end());
}

TEST_CASE("base64_encode", "[util][base64]")
{
  using namespace std::string_literals;
  using nbdl::util::base64_encode;

  CHECK(base64_encode("pleasure."s) == "cGxlYXN1cmUu"s);
  CHECK(base64_encode("leasure."s)  == "bGVhc3VyZS4="s);
  CHECK(base64_encode("easure."s)   == "ZWFzdXJlLg=="s);
  CHECK(base64_encode("asure."s)    == "YXN1cmUu"s);
  CHECK(base64_encode("sure."s)     == "c3VyZS4="s);

  CHECK(base64_encode(""s)     == ""s);
}

TEST_CASE("base64_decode", "[util][base64]")
{
  using namespace std::string_literals;
  using nbdl::util::base64_decode;

  CHECK(to_vec("pleasure."s) == base64_decode("cGxlYXN1cmUu"s));
  CHECK(to_vec("leasure."s)  == base64_decode(to_vec("bGVhc3VyZS4="s)));
  CHECK(to_vec("easure."s)   == base64_decode("ZWFzdXJlLg=="s));
  CHECK(to_vec("asure."s)    == base64_decode("YXN1cmUu"s));
  CHECK(to_vec("sure."s)     == base64_decode("c3VyZS4="s));

  CHECK(to_vec(""s) == base64_decode(""s));
  CHECK(to_vec(""s) == base64_decode("c3VyZS4$"s));
  CHECK(to_vec(""s) == base64_decode("^c3VyZS4"s));
  CHECK(to_vec(""s) == base64_decode("c3#VyZS4"s));
  CHECK(to_vec(""s) == base64_decode("c3VyZS4"s));
}
