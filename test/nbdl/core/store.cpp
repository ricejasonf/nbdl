//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include<macros/NBDL_ENTITY.hpp>
#include<Store.hpp>
#include<Path.hpp>

#include<catch.hpp>
#include<memory>

template<typename Path_>
using Store = nbdl::Store<
  nbdl::store::HashMap<Path_>,
  Path_ >;

struct Client
{
	int id;
};
struct MyEntity
{
	int id;
	int client_id;
};
using MyEntityPath = typename decltype(nbdl::path_type<int, Client, MyEntity>)::type;
namespace nbdl {
	NBDL_ENTITY(
		Client,
			id);
	NBDL_ENTITY(
		MyEntity,
			id,
			client_id);
}//nbdl

TEST_CASE("Access an uninitialized value from a store.", "[store]") 
{
	Store<MyEntityPath> store;
	MyEntityPath path = MyEntityPath(1, 5);

	bool result = store.get(
    path,
		[](nbdl::Unresolved) {
			return true;
		},
		[](auto) {
			return false;
		});

	CHECK(result == true);
}
TEST_CASE("Force assign and access a value from a store.", "[store]") 
{
	Store<MyEntityPath> store;
	MyEntityPath path = MyEntityPath(1, 5);
	MyEntity my_entity = { 5, 1 };
	store.forceAssign(path, my_entity);

	bool result = store.get(
		path,
		[](MyEntity entity) {
			CHECK(entity.id == 5);
			return true;	
		},
		[](auto) {
			return false;
		});

	CHECK(result == true);
}

TEST_CASE("Suggest a value to a store.", "[store]") 
{
	Store<MyEntityPath> store;
	MyEntityPath path = MyEntityPath(1, 5);
	MyEntity my_entity = { 5, 1 };
	store.suggestAssign(path, my_entity);

	bool result = store.get(
		path,
		[](MyEntity entity) {
			CHECK(entity.id == 5);
			return true;	
		},
		[](auto) {
			return false;
		});

	CHECK(result);
	
}

TEST_CASE("Suggest a value to a store where the value already exists.", "[store]") 
{
	Store<MyEntityPath> store;
	MyEntityPath path = MyEntityPath(1, 5);
	MyEntity my_entity_original = { 6, 1 };
	MyEntity my_entity = { 5, 1 };

	store.forceAssign(path, my_entity_original);

	store.suggestAssign(path, my_entity);

	bool result = store.get(
		path,
		[](MyEntity entity) {
			CHECK(entity.id == 6);
			return true;
		},
		[](auto) {
			return false;
		});

	CHECK(result);
}
