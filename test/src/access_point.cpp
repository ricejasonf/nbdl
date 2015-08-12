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

TEST_CASE("Create a root level access point.", "[api]") 
{
	using A = nbdl::AccessPoint<Client, int>;
	A a(5);
	REQUIRE(a.getKey() == 5);
}
TEST_CASE("Create a nested access point.", "[api]") 
{
	using A = nbdl::AccessPoint<Client, int>;
	using B = nbdl::AccessPoint<MyEntity, int, A>;
	B b = B(25, A(1));
	REQUIRE(b.getKey() == 25);
	REQUIRE(b.getParent().getKey() == 1);
}

/*
TEST_CASE("Access point greater that returns greater value.")
{
	using A = nbdl::AccessPoint<Client, int>;
	using B = nbdl::AccessPoint<MyEntity, int, A>;

	REQUIRE( (B(2, A(1)) > B(2, A(1))) == false);
	REQUIRE( (B(1, A(1)) > B(2, A(1))) == false);
	REQUIRE( (B(2, A(1)) > B(1, A(1))) == true);
	REQUIRE( (B(2, A(2)) > B(2, A(1))) == true);
	REQUIRE( (B(2, A(1)) > B(2, A(2))) == false);
}
*/
