//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <nbdl/entity_members.hpp>
#include<Store.hpp>
#include<Path.hpp>

#include <catch.hpp>
#include <memory>

template<typename Path_>
using Store = nbdl::store<
  nbdl::store_container::hash_map<Path_>,
  Path_ >;

struct client
{
	int id;
};
struct my_entity
{
	int id;
	int client_id;
};
using MyEntityPath = typename decltype(nbdl::path_type<int, client, my_entity>)::type;
namespace nbdl {
	NBDL_ENTITY(
		client,
			id);
	NBDL_ENTITY(
		my_entity,
			id,
			client_id);
}//nbdl

TEST_CASE("Access an uninitialized value from a store.", "[store]") 
{
  Store<MyEntityPath> store;
	MyEntityPath path = MyEntityPath(1, 5);

	bool result = store.get(
    path,
		[](nbdl::unresolved) {
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
	my_entity my_entity_ = { 5, 1 };
	store.force_assign(path, my_entity_);

	bool result = store.get(
		path,
		[](my_entity entity) {
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
	my_entity my_entity_ = { 5, 1 };
	store.suggest_assign(path, my_entity_);

	bool result = store.get(
		path,
		[](my_entity entity) {
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
	my_entity my_entity_original = { 6, 1 };
	my_entity my_entity_ = { 5, 1 };

	store.force_assign(path, my_entity_original);

	store.suggest_assign(path, my_entity_);

	bool result = store.get(
		path,
		[](my_entity entity) {
			CHECK(entity.id == 6);
			return true;
		},
		[](auto) {
			return false;
		});

	CHECK(result);
}
