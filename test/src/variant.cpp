#include<nbdl>
#include "catch.hpp"

TEST_CASE("MaxSizeOf should always return the largeset type.")
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

/*
TEST_CASE("Assign a value to a variant and use the callback interface to retrieve it.", "[api]") 
{
	using Number = nbdl::Variant<int, std::string, float>;
	
	Number number = 5;
	number.value(
		[&](int v) {
			REQUIRE(v == number);
		},
		[&](std::string) {
			REQUIRE(false);
		},
		[&](float) {
			REQUIRE(false);
		});
}
*/
