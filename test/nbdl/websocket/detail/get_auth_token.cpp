//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <catch.hpp>
#include <nbdl/websocket/detail/get_auth_token.hpp>
#include <string>

TEST_CASE("Parse cookies to obtain auth_token.", "[websocket][detail][get_auth_token]") 
{
  using nbdl::websocket::detail::get_auth_token;

  {
    std::string cookies = "nbdl_auth=Foo";
    CHECK("Foo" == get_auth_token(cookies).value);
  }
  {
    std::string cookies = "nbdl_auth=";
    CHECK("" == get_auth_token(cookies).value);
  }
  {
    std::string cookies = "asdfasdfad";
    CHECK("" == get_auth_token(cookies).value);
  }
  {
    std::string cookies = "foo=asdfasdfad; nbdl_auth=Foo";
    CHECK("Foo" == get_auth_token(cookies).value);
  }
  {
    std::string cookies = "foo=asdfasdfad; nbdl_authz=Foo";
    CHECK("" == get_auth_token(cookies).value);
  }
  {
    std::string cookies = "foo=asdfasdfad; asdfasfe=asdd; nbdl_auth=Foo";
    CHECK("Foo" == get_auth_token(cookies).value);
  }

  // invalid cookies
  {
    // happens to still work
    std::string cookies = "foo=asdfasdfad; asdfasdfasdf; nbdl_auth=Foo";
    CHECK("Foo" == get_auth_token(cookies).value);
  }
  {
    std::string cookies = "foo=asdfasdfad;asdfasdfasdf nbdl_auth=Foo";
    CHECK("" == get_auth_token(cookies).value);
  }
  {
    std::string cookies = "nbdl-auth:Foo;";
    CHECK("" == get_auth_token(cookies).value);
  }
  {
    std::string cookies = "nbdl=auth=Foo;";
    CHECK("" == get_auth_token(cookies).value);
  }
  {
    std::string cookies = "nbdl_auth=Foo;";
    CHECK("Foo" == get_auth_token(cookies).value);
  }
}
