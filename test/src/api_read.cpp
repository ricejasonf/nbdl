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
namespace nbdl {
	NBDL_ENTITY(
		Client,
			id);
	NBDL_ENTITY(
		MyEntity,
			id,
			client_id );
}//nbdl

struct MyApi
{
	using ClientPath = nbdl::AccessPoint<Client, int>;
	using MyEntityPath = nbdl::AccessPoint<MyEntity, int, ClientPath>;
	//NBDL_API_STUB_READ(ClientPath);
	//NBDL_API_STUB_READ(MyEntityPath);
};

//todo create the Server interface
//and have a wrapper that uses the api

TEST_CASE("Read an entity from an Api.", "[api]") 
{
	nbdl::Api<MyApi> api;
	MyApi::ClientPath clientPath(1);
	api.read(clientPath)
		.then([](Client client) {
			REQUIRE(client.id == 1);
		});
}
/*
TEST_CASE("Read an entity and then its child from an Api.", "[api]") 
{
	MyApi api;
	MyApi::ClientPath clientPath(1);
	MyApi::MyEntityPath myEntityPath(5, clientPath);
	api.read(MyApi::ClientPath(1))
		.then([clientPath](Client client) {
			REQUIRE(client.id == 1);
			return api.read(MyApi::MyEntityPath(5, clientPath))
		})
		.then(api.read(MyApi::MyEntityPath(5, MyApi::ClientPath(1))))
		.then([](MyEntity ent) {
			REQUIRE(ent.id == 5);
		});
}
*/
