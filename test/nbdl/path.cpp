//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//
#include<Traits.hpp>
#include<Path.hpp>
#include<macros/NBDL_ENTITY.hpp>

#include<catch.hpp>
#include<unordered_set> //for unique check

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
	using A = typename decltype(nbdl::path_type<int, Client>)::type;
	A a(5);
	CHECK(a.getKey() == 5);
}
TEST_CASE("Create a nested access point.", "[api]") 
{
	//using A = decltype(nbdl::path_type<int, Client>)::type;
	using B = decltype(nbdl::path_type<int, Client, MyEntity>)::type;
	B b = B(1, 25);
	static_assert(std::is_same<typename B::Entity, MyEntity>::value, "Path's entity is not correct.");
	CHECK(b.getKey<MyEntity>() == 25);
	CHECK(b.getKey() == 25);
	CHECK(b.parent().getKey() == 1);
}
TEST_CASE("Create a deeply nested access point.", "[api]") 
{
	using A = typename decltype(nbdl::path_type<int,
    Client,
		MyEntity,
		MyEntityAttribute,
		AttributeStatus>)::type;
	//lexicographic orderings
	A a(1, 2, 3, 4);

	CHECK(a.getKey<Client>() == 1);
	CHECK(a.getKey<MyEntity>() == 2);
	CHECK(a.getKey<MyEntityAttribute>() == 3);
	CHECK(a.getKey<AttributeStatus>() == 4);

	CHECK(a.getKey() == 4);
	CHECK(a.parent().getKey() == 3);
	CHECK(a.parent().parent().getKey() == 2);
	CHECK(a.parent().parent().parent().getKey() == 1);
}

/*
TEST_CASE("Hashed Paths should be unique.")
{
	using A = nbdl::path_type<int, Client>;
	using B = nbdl::path_type<int, Client, MyEntity>;

	using Set = std::unordered_set<B, typename B::HashFn, typename B::PredFn>;

	Set set;

	for (int i = 0; i < 1000; i++)
		for (int j = 0; j < 1000; j++)
			set.emplace(B(i, A(j)));
	CHECK(set.size() == 1000 * 1000);
}
*/
