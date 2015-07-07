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
	REQUIRE(a.key == 5);
}
TEST_CASE("Create a nested access point.", "[api]") 
{
	using A = nbdl::AccessPoint<Client, int>;
	using B = nbdl::AccessPoint<MyEntity, int, A>;
	B b = B(25, A(1));
	REQUIRE(b.key == 25);
	REQUIRE(b.parent.key == 1);
}
