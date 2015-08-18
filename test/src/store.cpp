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
using ClientPath = nbdl::Path<Client, int>;
using MyEntityPath = nbdl::Path<MyEntity, int, ClientPath>;
namespace nbdl {
	NBDL_ENTITY(
		Client,
			id);
	NBDL_ENTITY(
		MyEntity,
			id,
			client_id);
	template<>
	struct StorePath<MyEntity>
	{
		using Type = MyEntityPath;
	};
}//nbdl

TEST_CASE("Access an uninitialized value from a store.", "[store]") 
{
	nbdl::Store<MyEntity> store;
	MyEntityPath path = MyEntityPath(5, ClientPath(1));
	bool did_make_request = false;

	bool result = store.get([&](MyEntityPath ) { did_make_request = true; }, path,
		[](nbdl::Unresolved) {
			return true;
		},
		[]() {
			return false;
		});

	REQUIRE(result == true);
	REQUIRE(did_make_request == true);
}
TEST_CASE("Force assign and access a value from a store.", "[store]") 
{
	nbdl::Store<MyEntity> store;
	MyEntityPath path = MyEntityPath(5, ClientPath(1));
	MyEntity my_entity = { 5, 1 };
	bool did_make_request = false;
	store.forceAssign(path, my_entity);

	bool result = store.get(
		[&](MyEntityPath ) { 
			did_make_request = true; 
		}, 
		path,
		[](MyEntity entity) {
			REQUIRE(entity.id == 5);
			return true;	
		},
		[]() {
			return false;
		});

	REQUIRE(result == true);
	REQUIRE(did_make_request == false);
}

TEST_CASE("Suggest a value to a store.", "[store]") 
{
	nbdl::Store<MyEntity> store;
	MyEntityPath path = MyEntityPath(5, ClientPath(1));
	MyEntity my_entity = { 5, 1 };
	bool did_make_request = false;
	store.suggestAssign(path, my_entity);

	bool result = store.get(
		[&](MyEntityPath ) { 
			did_make_request = true; 
		}, 
		path,
		[](MyEntity entity) {
			REQUIRE(entity.id == 5);
			return true;	
		},
		[]() {
			return false;
		});

	REQUIRE(result == true);
	REQUIRE(did_make_request == false);
	
}

TEST_CASE("Suggest a value to a store where the value already exists.", "[store]") 
{
	nbdl::Store<MyEntity> store;
	MyEntityPath path = MyEntityPath(5, ClientPath(1));
	MyEntity my_entity = { 5, 1 };
	bool did_make_request = false;
	store.get(
		[&](MyEntityPath) {}, 
		path, 
		[](){});
	store.suggestAssign(path, my_entity);

	bool result = store.get(
		[&](MyEntityPath) { 
			did_make_request = true; 
		}, 
		path,
		[](MyEntity entity) {
			REQUIRE(entity.id == 5);
			return false;	
		},
		[]() {
			return true;
		});

	REQUIRE(result == true);
	REQUIRE(did_make_request == false);
}
