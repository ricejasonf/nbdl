//
// Copyright Jason Rice 2019
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at // http://www.boost.org/LICENSE_1_0.txt)
//

#include <catch.hpp>
#include <nbdl/app/detail/cookie_parser.hpp>
#include <string_view>

using namespace std::literals;
namespace detail = nbdl::app::detail;

TEST_CASE("Find value of NBDL_SESS from cookie string") {
  CHECK(detail::get_session_cookie("NBDL_SESS=foobar42"sv) ==
        "foobar42"sv);
  CHECK(detail::get_session_cookie("NBDL_SESS=foobar42;"sv) ==
        "foobar42"sv);
  CHECK(detail::get_session_cookie("NBDL_SESS=foobar42; "sv) == 
        "foobar42"sv);
  CHECK(detail::get_session_cookie("NBDL_SESS=foobar42; moo=cow"sv) ==
        "foobar42"sv);
  CHECK(detail::get_session_cookie("blah=blah; NBDL_SESS=foobar42; moo=cow"sv) ==
        "foobar42"sv);
  CHECK(detail::get_session_cookie("blah=blah;NBDL_SESS=foobar42; moo=cow"sv) ==
        "foobar42"sv);
  CHECK(detail::get_session_cookie("NBDL_SESS=asdfasdfasdfasdfasdfadf1234"sv) ==
        "asdfasdfasdfasdfasdfadf1234"sv);
}

TEST_CASE("Return empty string_view when NBDL_SESS not found") {
  CHECK(detail::get_session_cookie(""sv) ==
        std::string_view{});
  CHECK(detail::get_session_cookie(" ;"sv) ==
        std::string_view{});
  CHECK(detail::get_session_cookie("; "sv) ==
        std::string_view{});
  CHECK(detail::get_session_cookie(" ; "sv) ==
        std::string_view{});
  CHECK(detail::get_session_cookie("asdfjadfaksdjfasdf ") ==
        std::string_view{});
  CHECK(detail::get_session_cookie("asdfjadfaksdjfasdf;") ==
        std::string_view{});
  CHECK(detail::get_session_cookie("NBDL_SESS;") ==
        std::string_view{});
  CHECK(detail::get_session_cookie("blah=NBDL_SESS=moo;") ==
        std::string_view{});
}
