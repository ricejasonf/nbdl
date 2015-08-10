#include<string>
#include<nbdl>
#include "catch.hpp"

TEST_CASE("Unitialized variant should match Unresolved.", "[variant]") 
{
	using Number = nbdl::Variant<int, std::string, float>;
	Number number;
	REQUIRE(number.match(
		[](nbdl::Unresolved) {
			return 56;
		},
		[](std::string) {
			return 1;
		},
		[](int value) {
			return value;
		},
		[](float) {
			return 3;
		}) == 56);
}

TEST_CASE("Assign a value to a variant and use the callback interface to retrieve it.", "[variant]") 
{
	using Number = nbdl::Variant<int, std::string, float>;
	
	Number number = 512;
	REQUIRE(number.match(
		[](nbdl::Unresolved) {
			return 56;
		},
		[](std::string) {
			return 1;
		},
		[](int value) {
			return value;
		},
		[](float) {
			return 3;
		}) == 512);
}

TEST_CASE("Use the catch all to catch an unspecified tag", "[variant]")
{
	using Number = nbdl::Variant<int, std::string, float>;
	
	Number number = 185;
	REQUIRE(number.match(
		[](nbdl::Unresolved) {
			return 56;
		},
		[](std::string) {
			return 1;
		},
		[](float) {
			return 3;
		},
		[]() {
			return 123;
		}) == 123);
	number = 93;
	REQUIRE(number.match(
		[](nbdl::Unresolved) {
			return 56;
		},
		[](std::string) {
			return 1;
		},
		//not last position
		[]() {
			return 25;
		},
		[](float) {
			return 9;
		}) == 25);
}

//todo perhaps a matchRef function could allow mutable values in variants
TEST_CASE("Modify a struct's member in a variant", "[variant]")
{
	struct Person {
		std::string name_first;
		std::string name_last;
	};
	using MyVar = nbdl::Variant<Person, int, std::string, float>;
	
	MyVar var = Person { "Jason", "Rice" };
	var.match(
		[&](Person person) {
			person.name_last = "Ricez";
			var = person;
		});
	bool proof = var.match(
		[](Person person) {
			REQUIRE(person.name_last == "Ricez");
			return true;
		},
		[]() {
			return false;
		});
	REQUIRE(proof == true);
}

//todo remove tests of MaxSizeOf when std::aligned_union is used in Variant
TEST_CASE("MaxSizeOf should always return the largest type.", "[variant]")
{
	size_t size;
	struct LargeType
	{
		unsigned long x, y, z;
	};
	using nbdl::details::MaxSizeOf;
	size = MaxSizeOf<int, double>::value;
	REQUIRE(size == sizeof(double));
	size = MaxSizeOf<int, double, char>::value;
	REQUIRE(size == sizeof(double));
	size = MaxSizeOf<double, int>::value;
	REQUIRE(size == sizeof(double));
	size = MaxSizeOf<double, LargeType, int>::value;
	REQUIRE(size == sizeof(LargeType));
	size = MaxSizeOf<double, int, LargeType>::value;
	REQUIRE(size == sizeof(LargeType));
	size = MaxSizeOf<double, int, LargeType, int>::value;
	REQUIRE(size == sizeof(LargeType));
}
