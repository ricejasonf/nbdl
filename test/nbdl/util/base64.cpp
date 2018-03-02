//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/util/base64_encode.hpp>
#include <nbdl/util/base64_decode.hpp>

#include <algorithm>
#include <array>
#include <catch.hpp>
#include <string>
#include <vector>

namespace
{
  auto arbitrary_bytes_decoded = std::vector<unsigned char>{
    184, 167, 248, 113,  56,  19, 246,  74,
    101,  62, 122, 117,  43, 127, 151, 152
  };
  auto arbitrary_bytes_encoded = std::string("uKf4cTgT9kplPnp1K3+XmA==");
}

template <typename Buffer>
std::vector<unsigned char> to_vec(Buffer const& b)
{
  return std::vector<unsigned char>(b.begin(), b.end());
}

template <std::size_t n, typename Buffer>
auto to_array(Buffer const& b) -> std::array<unsigned char, n>
{
  if (b.size() != n) throw 0; // bad test would do this

  std::array<unsigned char, n> result{};
  std::copy(b.begin(), b.end(), result.begin());
  return result;
}

TEST_CASE("base64_encode", "[util]")
{
  using namespace std::string_literals;
  using nbdl::util::base64_encode;

  CHECK(base64_encode("pleasure."s) == "cGxlYXN1cmUu"s);
  CHECK(base64_encode("leasure."s)  == "bGVhc3VyZS4="s);
  CHECK(base64_encode("easure."s)   == "ZWFzdXJlLg=="s);
  CHECK(base64_encode("asure."s)    == "YXN1cmUu"s);
  CHECK(base64_encode("sure."s)     == "c3VyZS4="s);

  CHECK(base64_encode(std::vector<unsigned char>{255, 35, 254}) == "/yP+"s);
  CHECK(base64_encode(std::vector<unsigned char>{255, 35, 254, 123}) == "/yP+ew=="s);

  CHECK(base64_encode(""s)     == ""s);

  CHECK(base64_encode(arbitrary_bytes_decoded) == arbitrary_bytes_encoded)
}

TEST_CASE("base64_decode", "[util]")
{
  using namespace std::string_literals;
  using nbdl::util::base64_decode;

  CHECK(to_vec("pleasure."s) == base64_decode("cGxlYXN1cmUu"s));
  CHECK(to_vec("leasure."s)  == base64_decode("bGVhc3VyZS4="s));
  CHECK(to_vec("leasure."s)  == base64_decode(to_vec("bGVhc3VyZS4="s)));
  CHECK(to_vec("easure."s)   == base64_decode("ZWFzdXJlLg=="s));
  CHECK(to_vec("asure."s)    == base64_decode("YXN1cmUu"s));
  CHECK(to_vec("sure."s)     == base64_decode("c3VyZS4="s));

  CHECK(to_vec(""s) == base64_decode(""s));
  CHECK(to_vec(""s) == base64_decode("c3VyZS4$"s));
  CHECK(to_vec(""s) == base64_decode("^c3VyZS4"s));
  CHECK(to_vec(""s) == base64_decode("c3#VyZS4"s));
  CHECK(to_vec(""s) == base64_decode("c3VyZS4"s));

  CHECK((std::vector<unsigned char>{255, 35, 254}) == base64_decode("/yP+"s));
  CHECK((std::vector<unsigned char>{255, 35, 254, 123}) == base64_decode("/yP+ew=="s));

  CHECK(base64_decode(arbitrary_bytes_encoded) == arbitrary_bytes_decoded)
}

TEST_CASE("base64_decode output to existing container", "[util]")
{
  using namespace std::string_literals;
  using nbdl::util::base64_decode;

  {
    std::string result{};
    CHECK(base64_decode("ZWFzdXJlLg=="s, result));
    CHECK("easure."s  == result);
  }
  {
    std::array<unsigned char, 6> result{};
    CHECK(base64_decode("YXN1cmUu"s, result));
    CHECK(to_array<6>("asure."s)  == result);
  }
  { // fixed size target must have the correct length
    std::array<unsigned char, 5> result{};
    CHECK(not base64_decode("YXN1cmUu"s, result));
  }
  { // fixed size target must have the correct length
    std::array<unsigned char, 7> result{};
    CHECK(not base64_decode("YXN1cmUu"s, result));
  }
  {
    std::vector<unsigned char> result{};
    CHECK(base64_decode("YXN1cmUu"s, result));
    CHECK(to_vec("asure."s)  == result);
  }
  {
    std::string result{};
    CHECK(base64_decode(""s, result));
    CHECK(""s  == result);
  }
  {
    std::string result{};
    CHECK(base64_decode(""s, result));
    CHECK(""s  == result);
  }
  {
    std::array<unsigned char, 0> result{};
    CHECK(base64_decode(""s, result));
    CHECK(to_array<0>(""s)  == result);
  }
  {
    std::string result{};
    CHECK(not base64_decode("^c3VyZS4"s, result));
  }
}
