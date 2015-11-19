//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include<memory>
#include<nbdl>
#include "catch.hpp"

template<typename Path_>
using Store = nbdl::Store<
  nbdl::store::HashMap<Path_>,
  nbdl::store_emitter::HashMap<Path_, nbdl::ListenerHandlerDummy<>>,
  nbdl::ListenerHandlerDummy<>,
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
//using ClientPath = typename decltype(nbdl::path_type<int, Client>)::type;
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
	bool did_make_request = false;

	bool result = store.get([&](MyEntityPath ) { did_make_request = true; }, path,
		[](nbdl::Unresolved) {
			return true;
		},
		[](auto) {
			return false;
		});

	CHECK(result == true);
	CHECK(did_make_request == true);
}
TEST_CASE("Force assign and access a value from a store.", "[store]") 
{
	Store<MyEntityPath> store;
	MyEntityPath path = MyEntityPath(1, 5);
	MyEntity my_entity = { 5, 1 };
	bool did_make_request = false;
	store.forceAssign(path, my_entity);

	bool result = store.get(
		[&](MyEntityPath ) { 
			did_make_request = true; 
		}, 
		path,
		[](MyEntity entity) {
			CHECK(entity.id == 5);
			return true;	
		},
		[](auto) {
			return false;
		});

	CHECK(result == true);
	CHECK(did_make_request == false);
}

TEST_CASE("Suggest a value to a store.", "[store]") 
{
	Store<MyEntityPath> store;
	MyEntityPath path = MyEntityPath(1, 5);
	MyEntity my_entity = { 5, 1 };
	bool did_make_request = false;
	store.suggestAssign(path, my_entity);

	bool result = store.get(
		[&](MyEntityPath ) { 
			did_make_request = true; 
		}, 
		path,
		[](MyEntity entity) {
			CHECK(entity.id == 5);
			return true;	
		},
		[](auto) {
			return false;
		});

	CHECK(result);
	CHECK_FALSE(did_make_request);
	
}

TEST_CASE("Suggest a value to a store where the value already exists.", "[store]") 
{
	Store<MyEntityPath> store;
	MyEntityPath path = MyEntityPath(1, 5);
	MyEntity my_entity_original = { 6, 1 };
	MyEntity my_entity = { 5, 1 };
	bool did_make_request = false;

	store.forceAssign(path, my_entity_original);

	store.suggestAssign(path, my_entity);

	bool result = store.get(
		[&](MyEntityPath) { 
			did_make_request = true; 
		}, 
		path,
		[](MyEntity entity) {
			CHECK(entity.id == 6);
			return true;
		},
		[](auto) {
			return false;
		});

	CHECK(result);
	CHECK_FALSE(did_make_request);
}
