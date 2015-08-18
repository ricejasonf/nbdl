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
	using A = nbdl::AccessPoint;
	using Path = nbdl::CreatePath;
	using Read = nbdl::api::Read;

	using Definition = nbdl::ApiDefinition<
		AccessPoint<
			Path<Client, MyEntity>, 
			Actions<Read>
		>
	>;
};

struct TestServer
{
	template<typename Fn>
	void read(MyApi::ClientPath path, Fn fn)
	{
		Client client;
		client.id = path.key;
		fn(client);
	}
};

//todo create the Server interface
//and have a wrapper that uses the api

TEST_CASE("Start a read from a Context.", "[api]") 
{
	nbdl::Context<Server, MyApi> ctz;
	MyApi::ClientPath clientPath(1);
	ctx.read(clientPath,
		[](nbdl::Unresolved) {
			REQUIRE(true)
		},
		[](nbdl::NotFound) {
			REQUIRE(false)
		},
		[](const Client& client) {
			REQUIRE(false)
			//REQUIRE(client.id == 1);
		});
}
