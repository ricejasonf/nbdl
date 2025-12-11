//
// Copyright Jason Rice 2025
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/bind_variant.hpp>
#include <nbdl/match.hpp>
#include <nbdl/variant_holder.hpp>
#include <nbdl/tags.hpp>

#include <catch.hpp>
#include <string>

namespace hana = boost::hana;

struct some_tag { };

TEST_CASE("Unitialized variant should match the first alternative.", "[variant_holder]") {
	using Number = nbdl::variant_holder<
    std::variant<nbdl::unresolved, int, std::string, some_tag>>;
	Number number;
  int result = 0;

	nbdl::match(number, nbdl::variant_value, hana::overload(
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
  ));
  CHECK(result == 56);
}

TEST_CASE("Assign a value to a variant and use the "
          "callback interface to retrieve it.",
          "[variant_holder]") {
	using Number = nbdl::variant_holder<
    std::variant<nbdl::unresolved, int, std::string, some_tag>>;
	
	Number number = Number(512);
  int result = 0;

	nbdl::match(number, nbdl::variant_value, hana::overload(
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
  ));
  CHECK(result == 512);
}

TEST_CASE("Modify a struct's member in a variant", "[variant]")
{
	struct person {
		std::string name_first;
		std::string name_last;
	};
	using MyVar = nbdl::variant_holder<
    std::variant<person, int, std::string, some_tag, std::vector<char>>>;
	
	MyVar var = MyVar(person{"Jason", "Rice"});

  nbdl::match(var, nbdl::variant_value, hana::overload(
		[&](person& p) {
			p.name_last = "Ricez";
		}, nbdl::noop));

  bool result = false;

  nbdl::match(var, nbdl::variant_value, hana::overload(
		[&](person const& p) {
			CHECK(p.name_last == "Ricez");
			result = true;
		},
		[&](auto const&) {
			result = false;
		}));

	CHECK(result);
}

TEST_CASE("variant_holder as a nbdl::State", "[variant_holder][State]")
{
	using Number = nbdl::variant_holder<
      std::variant<nbdl::unresolved, some_tag, int, std::string>>;
	Number number = Number(512);
  size_t result_type_id = -1;
  int result = 0;

  nbdl::match(number, nbdl::variant_index, [&](size_t type_id) {
      result_type_id = type_id;
    });

	nbdl::match(number, nbdl::variant_value, hana::overload(
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
      }));


  CHECK(result == 512);
  CHECK(result_type_id == 2);
}
