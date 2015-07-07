#include <string>

/*
#include <EntityTraits.hpp>
#include <MemberSet.hpp>
#include <Bind.hpp>
#include <Validate.hpp>
*/

#include <nbdl>
/*
#include <binders/JsonRead.h>
#include <binders/JsonWrite.h>
#include <binders/JsonCppErrorBinder.hpp>
*/
#include <jsoncpp/json/json.h>
#include "assets/AccountEntity.hpp"

void fromString(std::string &json, Account &account)
{
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(json, root, false))
		throw std::runtime_error("JSON parse error");
	nbdl::JsonRead r(root);
	nbdl::bind(r, account);
}

std::string toString(Account &account)
{
	Json::StyledWriter writer;
	Json::Value root;
	nbdl::JsonWrite r(root);

	nbdl::bind(r, account);
	return writer.write(root);
}

#include "catch.hpp"
TEST_CASE("Entity can read and write to and from JSON.", "[bind][json]") 
{
	Account account = Account();
	std::string inputJson, outputJson;

	inputJson = account_entity_json_test_string;

	fromString(inputJson, account);
    REQUIRE(account.nameFirst == "Jason");
    REQUIRE(account.nameLast == "Rice");
    REQUIRE(account.age == 34);
	REQUIRE(toString(account) == "{\n   \"address\" : {\n      \"city\" : \"Las Vegas\",\n      \"line1\" : \"  123 Spork Rd.  \",\n      \"line2\" : \"\",\n      \"state\" : \"NV\",\n      \"zipCode\" : \"89015\"\n   },\n   \"age\" : 34,\n   \"email\" : \"ricejasonf@gmail.com\",\n   \"food\" : {\n      \"foodGroup\" : {\n         \"name\" : \"Fruits\"\n      },\n      \"id\" : 1,\n      \"name\" : \"Banana\"\n   },\n   \"nameFirst\" : \"Jason\",\n   \"nameLast\" : \"Rice\",\n   \"phoneNumber\" : \"7024569874\"\n}\n");
}
