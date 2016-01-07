//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include<nbdl>
#include<catch.hpp>

struct Type1 {};
struct Type2 {};
struct Type3 {};
struct Type4 {};
template<typename T>
int getInt(T tag)
{
	return  nbdl::VariantCallback::call(tag,
		[](Type2) {
			return 2;
		},
		[](Type1) {
			return 1;
		},
		[](Type4) {
			return 4;
		});
}
template<typename T>
void modifyInt(int &i, T tag)
{
	return  nbdl::VariantCallback::call(tag,
		[&](Type2) {
			i = 2;
		},
		[&](Type1) {
			i = 1;
		},
		[&](Type4) {
			i = 4;
		});
}

TEST_CASE("VariantCallback should match lambda based on argument type and do return a value.")
{
	REQUIRE(getInt(Type1{}) == 1);
	REQUIRE(getInt(Type2{}) == 2);
	REQUIRE(getInt(Type4{}) == 4);
	//Type3 doesn't compile because there is no match
	//this prevents returning an undefined value (for not void return types)
	//REQUIRE(getInt(Type3{}) == 3);
};
TEST_CASE("VariantCallback should match lambda based on argument type and do not return a value.")
{
	int value = 0;
	modifyInt(value, Type1{});
	REQUIRE(value == 1);
	modifyInt(value, Type2{});
	REQUIRE(value == 2);
	modifyInt(value, Type4{});
	REQUIRE(value == 4);
};
TEST_CASE("VariantCallback returning void should allow unspecified types and do nothing.")
{
	int value = 56;
	modifyInt(value, Type3{});
	REQUIRE(value == 56);
};
