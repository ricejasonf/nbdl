//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include<functional>
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

using MyContext = nbdl::Context<
	TestClient,
	nbdl::ListenerHandlerDummy<>,
	nbdl_def::Api<
		nbdl_def::AccessPoint<
			OnlySupportedPath,
			nbdl_def::Actions<nbdl_def::Read>
		>
	>>;

TEST_CASE("Read an object from a context.", "[context]") 
{
	bool result;
	auto ctx = MyContext::create(TestClient{});

	result = ctx->read(OnlySupportedPath(1, 5),
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
	auto ctx = MyContext::create(TestClient{});

	result = ctx->read(OnlySupportedPath(1, 6),
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

class TestClientAsync
{
	static std::function<void()> m_fn;

	public:

	static void flush__()
	{
		m_fn();
	}

	template<typename Fn>
	void read(OnlySupportedPath path, Fn fn)
	{
		m_fn = [path, fn]() {
			if (path.getKey<Client>() == 1 && path.getKey<MyEntity>() == 5)
			{
				MyEntity my_entity = { 5, 1 };
				fn(my_entity);
			}
			else
			{
				fn(nbdl::NotFound{});
			}
		};
	}
};
std::function<void()> TestClientAsync::m_fn = [](){};

using MyContextAsync = nbdl::Context<
	TestClientAsync,
	nbdl::ListenerHandlerDummy<>,
	nbdl_def::Api<
		nbdl_def::AccessPoint<
			OnlySupportedPath,
			nbdl_def::Actions<nbdl_def::Read>
		>
	>>;

TEST_CASE("Context should emit change to listener", "[context]")
{
	int function_was_called = false;
	int result = 0;
	auto ctx = MyContextAsync::create(TestClientAsync{});
	OnlySupportedPath path(1, 5);

	auto listener = ctx->makeListener(path, [&]() {
		function_was_called = true;
		result = ctx->read(path,
			[](nbdl::Unresolved) {
				return 1;
			},
			[](MyEntity) {
				return 2;
			},
			[](nbdl::NotFound) {
				return 3;
			});
	});

	ctx->read(path, [](){});
	CHECK_FALSE(result); //does not emit for unresolved
	TestClientAsync::flush__();
	CHECK(function_was_called);
	CHECK(result == 2); //MyEntity
}

TEST_CASE("Context should not emit to a listener that has been destroyed.", "[context]")
{
	int function_was_called = false;
	int result = 0;
	OnlySupportedPath path(1, 5);

	{
		auto ctx = MyContextAsync::create(TestClientAsync{});
		/*
			in this test if the listener fails
			to unregister it would result in UB.
		*/
		auto listener = ctx->makeListener(path, [&]() {
			function_was_called = true;
			result = ctx->read(path,
				[](nbdl::Unresolved) {
					return 1;
				},
				[](MyEntity) {
					return 2;
				},
				[](nbdl::NotFound) {
					return 3;
				});
		});
		ctx->read(path, [](){});
	} //listener is destroyed and should unregister from the context

	CHECK_FALSE(result); //does not emit for unresolved
	TestClientAsync::flush__();
	CHECK_FALSE(function_was_called);
	CHECK(result == 0);
}
