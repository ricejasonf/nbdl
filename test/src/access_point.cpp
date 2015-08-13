#include<unordered_set> //for unique check
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

/* test takes a long time
TEST_CASE("Hashed AccessPoints should be unique.")
{
	using A = nbdl::AccessPoint<Client, int>;
	using B = nbdl::AccessPoint<MyEntity, int, A>;

	using Set = std::unordered_set<B, typename B::HashFn, typename B::PredFn>;

	Set set;

	for (int i = 0; i < 1000; i++)
		for (int j = 0; j < 1000; j++)
			set.emplace(B(i, A(j)));
	REQUIRE(set.size() == 1000 * 1000);
}
*/
