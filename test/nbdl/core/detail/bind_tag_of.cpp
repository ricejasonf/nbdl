//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <nbdl/detail/bind_tag_of.hpp>
#include <nbdl/string.hpp>
#include <nbdl/variant.hpp>

#include <boost/hana.hpp>
#include <catch.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace hana = boost::hana;

struct foo_tag { };
constexpr auto foo_s = BOOST_HANA_STRING("foo");

int main()
{
  static_assert(std::is_empty<hana::tuple<>>{});
  static_assert(!std::is_empty<hana::tuple<hana::int_<42>>>{});
  static_assert(!std::is_empty<hana::tuple<hana::tuple<>>>{});
  using input = hana::tuple<
    int
  , float
  , std::vector<nbdl::string>
  , std::unordered_map<int, std::string>
  , hana::type<int>
  , hana::type<nbdl::string>
  , decltype(hana::typeid_(foo_s))
  , nbdl::string
  , hana::int_<42>
  , decltype(hana::make_map(hana::make_pair(foo_s, 5)))
  , decltype(hana::make_map())
  , hana::tuple<>
  , hana::tuple<hana::int_<42>>
  , hana::tuple<int, float>
  , foo_tag
  , nbdl::variant<foo_tag, nbdl::string>
  >;

  namespace tag = nbdl::_b::tag;

  using expected = tag::sequence<
    int
  , float
  , tag::container<tag::string>
  , tag::container<tag::sequence<int, tag::dynamic_buffer>>
  , tag::type<int>
  , tag::type<nbdl::string>
  , tag::type<std::decay_t<decltype(foo_s)>>
  , tag::string
  , tag::constant<42>
  , tag::map<tag::map_node<tag::ct_string<'f', 'o', 'o'>, int>>
  , tag::map<>
  , tag::sequence<>
  , tag::sequence<tag::constant<42>>
  , tag::sequence<int, float>
  , foo_tag
  , tag::variant<foo_tag, tag::string>
  >;

  using result = nbdl::detail::bind_tag_of<input>;

  BOOST_HANA_CONSTANT_ASSERT(hana::type_c<result> == hana::type_c<expected>);
}
