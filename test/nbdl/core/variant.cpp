//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <nbdl/variant.hpp>

#include <catch.hpp>
#include <string>

struct some_tag {};

TEST_CASE("Unitialized variant should match Unresolved.", "[variant]")
{
	using Number = nbdl::variant<int, std::string, some_tag>;
	Number number;
	CHECK(number.match(
		[](nbdl::unresolved) {
			return 56;
		},
		[](std::string) {
			return 1;
		},
		[](int value) {
			return value;
		},
		[](some_tag) {
			return 3;
		}) == 56);
}

TEST_CASE("Assign a value to a variant and use the callback interface to retrieve it.", "[variant]")
{
	using Number = nbdl::variant<int, std::string, some_tag>;
	
	Number number = 512;
	CHECK(number.match(
		[](nbdl::unresolved) {
			return 56;
		},
		[](std::string) {
			return 1;
		},
		[](int value) {
			return value;
		},
		[](some_tag) {
			return 3;
		}) == 512);
}

TEST_CASE("Copy a variant value.", "[variant]") 
{
	using Number = nbdl::variant<int, some_tag>;
	Number number1, number2;
	number1 = 123;
	number2 = Number(number1);
	CHECK(number2.match(
		[](nbdl::unresolved) {
			return 56;
		},
		[](std::string) {
			return 1;
		},
		[](int value) {
			return value;
		},
		[](some_tag) {
			return 3;
		}) == 123);
}

TEST_CASE("Use the catch all to catch an unspecified tag", "[variant]")
{
	using Number = nbdl::variant<int, std::string, some_tag>;
	
	Number number = 185;
	CHECK(number.match(
		[](nbdl::unresolved) {
			return 56;
		},
		[](std::string) {
			return 1;
		},
		[](some_tag) {
			return 3;
		},
		[](auto) {
			return 123;
		}) == 123);
	number = 93;
	CHECK(number.match(
		[](nbdl::unresolved) {
			return 56;
		},
		[](std::string) {
			return 1;
		},
		//not last position
		[](auto) {
			return 25;
		},
		[](some_tag) {
			return 9;
		}) == 25);
}
TEST_CASE("Catch should not be called if there is a valid match.", "[variant]")
{
	using Number = nbdl::variant<int, std::string, some_tag>;
	Number number = 185;

	CHECK(number.match(
		[](nbdl::unresolved) {
			return 56;
		},
		[](int) {
			return 76;
		},
		[](auto) {
			return 25;
		}) == 76);
}

TEST_CASE("Modify a struct's member in a variant", "[variant]")
{
	struct person {
		std::string name_first;
		std::string name_last;
	};
	using MyVar = nbdl::variant<person, int, std::string, some_tag, std::vector<char>>;
	
	MyVar var = person{ "Jason", "Rice" };

	var.match(
		[&](person p) {
			p.name_last = "Ricez";
			var = p;
		}, nbdl::noop);
	bool result = var.match(
		[](person p) {
			CHECK(p.name_last == "Ricez");
			return true;
		},
		[](auto const&) {
			return false;
		});
	CHECK(result);
}

TEST_CASE("variant as a nbdl::Store", "[variant][Store]")
{
  struct some_path { };
  using optional_string = nbdl::optional<std::string>;
  using very_optional_string = nbdl::optional<optional_string>;

  {
    optional_string x = std::string("Hello Worldz!");
    very_optional_string y = x;

    std::string result{};

    nbdl::match(y, some_path{}
    , [&](std::string const& value) { result = value; }
    , [&](auto&&) { result = std::string("not this"); }
    );

    CHECK(result == std::string("Hello Worldz!"));
  }
  {
    optional_string x{};
    very_optional_string y = x;

    std::string result{};

    nbdl::match(y, some_path{}
    , [&](std::string const&) { result = std::string("not this"); }
    , [&](auto&&) { result = std::string("should match this"); }
    );

    CHECK(result == std::string("should match this"));
  }
}
