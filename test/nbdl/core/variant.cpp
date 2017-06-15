//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <nbdl/variant.hpp>

#include <boost/hana/tuple.hpp>
#include <catch.hpp>
#include <string>

namespace hana = boost::hana;

struct some_tag {};

TEST_CASE("Unitialized variant should match Unresolved.", "[variant]")
{
	using Number = nbdl::variant<int, std::string, some_tag>;
	Number number;
  CHECK(number.is<nbdl::unresolved>());

  int result = 0;

	number.match(
		[&](nbdl::unresolved) {
			result = 56;
		},
		[&](std::string) {
			result = 1;
		},
		[&](int value) {
			result = value;
		},
		[&](some_tag) {
			result = 3;
		}
  );
  CHECK(result == 56);
}

TEST_CASE("Assign a value to a variant and use the callback interface to retrieve it.", "[variant]")
{
	using Number = nbdl::variant<int, std::string, some_tag>;
	
	Number number = 512;
  CHECK(number.is<int>());

  int result = 0;

	number.match(
		[&](nbdl::unresolved) {
			result = 56;
		},
		[&](std::string) {
			result = 1;
		},
		[&](int value) {
			result = value;
		},
		[&](some_tag) {
			result = 3;
		}
  );
  CHECK(result == 512);
}

TEST_CASE("Copy a variant value.", "[variant]") 
{
	using Number = nbdl::variant<int, some_tag>;
	Number number1, number2;
	number1 = 123;
  CHECK(number1.is<int>());

	number2 = Number(number1);
  CHECK(number2.is<int>());

  int result = 0;

	number2.match(
		[&](nbdl::unresolved) {
			result = 56;
		},
		[&](std::string) {
			result = 1;
		},
		[&](int value) {
			result = value;
		},
		[&](some_tag) {
			result = 3;
		}
  );
  CHECK(result == 123);
}

TEST_CASE("Use the catch all to catch an unspecified tag", "[variant]")
{
	using Number = nbdl::variant<int, std::string, some_tag>;
	
	Number number = 185;
  CHECK(number.is<int>());

  int result = 0;

	number.match(
		[&](nbdl::unresolved) {
			result = 56;
		},
		[&](std::string) {
			result = 1;
		},
		[&](some_tag) {
			result = 3;
		},
		[&](auto) {
			result = 123;
		}
  );
  CHECK(result == 123);

	number = 93;
  CHECK(number.is<int>());

  result = 0;

	number.match(
		[&](nbdl::unresolved) {
			result = 56;
		},
		[&](std::string) {
			result = 1;
		},
		//not last position
		[&](auto) {
			result = 25;
		},
		[&](some_tag) {
			result = 9;
		}
  );
  CHECK(result == 25);
}
TEST_CASE("Catch should not be called if there is a valid match.", "[variant]")
{
	using Number = nbdl::variant<int, std::string, some_tag>;
	Number number = 185;
  CHECK(number.is<int>());

  int result = 0;

	number.match(
		[&](nbdl::unresolved) {
			result = 56;
		},
		[&](int) {
			result = 76;
		},
		[&](auto) {
			result = 25;
		}
  );
  CHECK(result == 76);
}

TEST_CASE("Modify a struct's member in a variant", "[variant]")
{
	struct person {
		std::string name_first;
		std::string name_last;
	};
	using MyVar = nbdl::variant<person, int, std::string, some_tag, std::vector<char>>;
	
	MyVar var = person{ "Jason", "Rice" };
  CHECK(var.is<person>());

	var.match(
		[&](person p) {
			p.name_last = "Ricez";
			var = p;
		}, nbdl::noop);

  bool result = false;

	var.match(
		[&](person p) {
			CHECK(p.name_last == "Ricez");
			result = true;
		},
		[&](auto const&) {
			result = false;
		});

	CHECK(result);
}

TEST_CASE("variant as a nbdl::Store", "[variant][Store]")
{
  struct some_key { };
  using optional_string = nbdl::optional<std::string>;
  using very_optional_string = nbdl::optional<optional_string>;

  // nbdl::variant just proxies the match call to its contained
  // value so it works recursively on nested nbdl::variants
  {
    optional_string x = std::string("Hello Worldz!");
    very_optional_string y = x;

    std::string result{};

    nbdl::match(y, some_key{}, hana::overload_linearly(
      [&](std::string const& value) { result = value; }
    , [&](auto&&) { result = std::string("not this"); }
    ));

    CHECK(result == std::string("Hello Worldz!"));
  }
  {
    optional_string x{};
    very_optional_string y = x;

    std::string result{};

    nbdl::match(y, some_key{}, hana::overload_linearly(
      [&](std::string const&) { result = std::string("not this"); }
    , [&](auto&&) { result = std::string("should match this"); }
    ));

    CHECK(result == std::string("should match this"));
  }
}

TEST_CASE("Apply action update raw.", "[variant][apply_action]")
{
  auto store = nbdl::variant<int, std::string>{};

  nbdl::apply_action(store, std::string("OK!"));

  std::string result("FAIL!");

  store.match([&](std::string const& value) { result = value; }, nbdl::noop);
  CHECK(result == std::string("OK!"));
}
