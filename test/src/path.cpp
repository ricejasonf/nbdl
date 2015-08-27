//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include<unordered_set> //for unique check
#include<nbdl>
#include "catch.hpp"

struct Client
{
	int id;
};
struct MyEntity
{
	int id;
	int client_id;
};
struct MyEntityAttribute
{
	int id;
	int my_entity_id;
};
struct AttributeStatus
{
	int id;
	int attribute_id;
};
namespace nbdl {
	NBDL_ENTITY(
		Client,
			id);
	NBDL_ENTITY(
		MyEntity,
			id,
			client_id );
	NBDL_ENTITY(
		MyEntityAttribute,
			id,
			my_entity_id );
	NBDL_ENTITY(
		AttributeStatus,
			id,
			attribute_id );
}//nbdl

TEST_CASE("Create a root level access point.", "[api]") 
{
	using A = nbdl::Path<Client, int>;
	A a(5);
	CHECK(a.getKey() == 5);
}
TEST_CASE("Create a nested access point.", "[api]") 
{
	using A = nbdl::Path<Client, int>;
	using B = nbdl::Path<MyEntity, int, A>;
	B b = B(25, A(1));
	static_assert(std::is_same<typename B::Entity, MyEntity>::value, "Path's entity is not correct.");
	CHECK(b.getKey<MyEntity>() == 25);
	CHECK(b.getKey() == 25);
	CHECK(b.getParent().getKey() == 1);
}
TEST_CASE("Create a deeply nested access point.", "[api]") 
{
	using A = typename nbdl::CreatePath<
		nbdl::Path<Client, int>,
		MyEntity,
		nbdl::Path<MyEntityAttribute, int>,
		nbdl::Path<AttributeStatus, int>
	>::Type;
	//lexicographic orderings
	A a(1, 2, 3, 4);

	CHECK(a.getKey<Client>() == 1);
	CHECK(a.getKey<MyEntity>() == 2);
	CHECK(a.getKey<MyEntityAttribute>() == 3);
	CHECK(a.getKey<AttributeStatus>() == 4);

	CHECK(a.getKey() == 4);
	CHECK(a.getParent().getKey() == 3);
	CHECK(a.getParent().getParent().getKey() == 2);
	CHECK(a.getParent().getParent().getParent().getKey() == 1);
}

/* test takes a long time
TEST_CASE("Hashed Paths should be unique.")
{
	using A = nbdl::Path<Client, int>;
	using B = nbdl::Path<MyEntity, int, A>;

	using Set = std::unordered_set<B, typename B::HashFn, typename B::PredFn>;

	Set set;

	for (int i = 0; i < 1000; i++)
		for (int j = 0; j < 1000; j++)
			set.emplace(B(i, A(j)));
	CHECK(set.size() == 1000 * 1000);
}
*/
