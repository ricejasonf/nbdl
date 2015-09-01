//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
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

using OnlySupportedPath = typename nbdl::CreatePath<Client, MyEntity>::Type;
static_assert(std::is_same<typename OnlySupportedPath::Entity, MyEntity>::value, "Only supported type entity is not MyEntity.");
using MyApi = nbdl::ApiDefinition<
	nbdl::AccessPoint<
		OnlySupportedPath,
		nbdl::Actions<nbdl::actions::Read>
	>
>;

struct TestClient
{
	template<typename Fn>
	void read(OnlySupportedPath path, Fn fn)
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
	nbdl::Context<TestClient, MyApi> ctx(TestClient{});

	result = ctx.read(OnlySupportedPath(1, 5),
		[](nbdl::Unresolved) {
			return false;
		},
		[](nbdl::NotFound) {
			return false;
		},
		[](MyEntity m) {
			return (m.id == 5 && m.client_id == 1);
		});

	CHECK(result);
}

TEST_CASE("Context should propagate NotFound from server callback.", "[context]") 
{
	bool result = false;
	nbdl::Context<TestClient, MyApi> ctx(TestClient{});

	result = ctx.read(OnlySupportedPath(1, 6),
		[](nbdl::Unresolved) {
			return false;
		},
		[](nbdl::NotFound) {
			return true;
		},
		[](MyEntity) {
			return false;
		});

	CHECK(result);
}
