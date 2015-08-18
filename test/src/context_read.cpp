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

using MyApi = nbdl::ApiDefinition<
	nbdl::AccessPoint<
		nbdl::CreatePath<Client, MyEntity>, 
		nbdl::Actions<Read>
	>
>;

struct TestServer
{
	template<typename Fn>
	void read(nbdl::CreatePath<Client, MyEntity> path, Fn fn)
	{
		if (path.getKey<Client>() == 1 && path.getKey<MyEntity>() == 5)
		{
			MyEntity my_entity = { 5, 1 };
			fn(my_entity);
		}
		else
		{
			fn(nbdl::NotFound{});
		}
	}
};

TEST_CASE("Read an object from a context.", "[context]") 
{
	bool result;
	TestServer server;
	nbdl::Context<Server, MyApi> ctx(server);

	result = ctx.read(nbdl::CreatePath<Client, MyEntity> path(1, 5),
		[](nbdl::Unresolved) {
			return false;
		},
		[](nbdl::NotFound) {
			return false;
		},
		[](MyEntity m) {
			return (m.id == 5 && m.client_id = 1)
		});
	CHECK(result);
}
TEST_CASE("Context should propagate NotFound from server callback.", "[context]") 
{
	bool result;
	TestServer server;
	nbdl::Context<Server, MyApi> ctz(server);

	result = ctx.read(nbdl::CreatePath<Client, MyEntity> path(1, 6),
		[](nbdl::Unresolved) {
			return false;
		},
		[](nbdl::NotFound) {
			return true;
		},
		[](MyEntity m) {
			return false;
		});
	CHECK(result);
}
TEST_CASE("Context should return NotFound for invalid paths.", "[context]") 
{
	bool result;
	TestServer server;
	nbdl::Context<Server, MyApi> ctz(server);

	result = ctx.read(nbdl::CreatePath<MyEntity> path(5),
		[](nbdl::Unresolved) {
			return false;
		},
		[](nbdl::NotFound) {
			return true;
		},
		[](MyEntity m) {
			return false;
		});
	CHECK(result);
}
