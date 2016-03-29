//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//

#include <nbdl/entity_members.hpp>
#include<Path.hpp>

#include <catch.hpp>
#include <unordered_set> //for unique check

struct client
{
	int id;
};
struct my_entity
{
	int id;
	int client_id;
};
struct my_entity_attribute
{
	int id;
	int my_entity_id;
};
struct attribute_status
{
	int id;
	int attribute_id;
};
namespace nbdl {
	NBDL_ENTITY(
		client,
			id);
	NBDL_ENTITY(
		my_entity,
			id,
			client_id );
	NBDL_ENTITY(
		my_entity_attribute,
			id,
			my_entity_id );
	NBDL_ENTITY(
		attribute_status,
			id,
			attribute_id );
}//nbdl

TEST_CASE("Create a root level access point.", "[api]") 
{
	using A = typename decltype(nbdl::path_type<int, client>)::type;
	A a(5);
	CHECK(a.get_key() == 5);
}
TEST_CASE("Create a nested access point.", "[api]") 
{
	//using A = decltype(nbdl::path_type<int, Client>)::type;
	using B = decltype(nbdl::path_type<int, client, my_entity>)::type;
	B b = B(1, 25);
	static_assert(std::is_same<typename B::Entity, my_entity>::value, "Path's entity is not correct.");
	CHECK(b.get_key<my_entity>() == 25);
	CHECK(b.get_key() == 25);
	CHECK(b.parent().get_key() == 1);
}
TEST_CASE("Create a deeply nested access point.", "[api]") 
{
	using A = typename decltype(nbdl::path_type<int,
    client,
		my_entity,
		my_entity_attribute,
		attribute_status>)::type;
	//lexicographic orderings
	A a(1, 2, 3, 4);

	CHECK(a.get_key<client>() == 1);
	CHECK(a.get_key<my_entity>() == 2);
	CHECK(a.get_key<my_entity_attribute>() == 3);
	CHECK(a.get_key<attribute_status>() == 4);

	CHECK(a.get_key() == 4);
	CHECK(a.parent().get_key() == 3);
	CHECK(a.parent().parent().get_key() == 2);
	CHECK(a.parent().parent().parent().get_key() == 1);
}
