#include <iostream>
#include <string>

#include <EntityTraits.hpp>
#include <MemberSet.hpp>
#include <Bind.hpp>
#include <Validate.hpp>

#include <binders/JsonRead.h>
#include <binders/JsonWrite.h>
#include <binders/JsonCppErrorBinder.hpp>
//#include <binders/JsonCppApplyDiff.hpp>
#include <jsoncpp/json/json.h>


#include "Account.h"

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

std::string validate(Account &account)
{
	Json::StyledWriter writer;
	Json::Value root;
	nbdl::binders::JsonCppValidationErrors e(root);

	nbdl::validate(e, account);
	return writer.write(root);
}

int main()
{
	Json::StyledWriter writer;
	auto account = Account();
	std::string inputJson, outputJson;
	for (std::string line; std::getline(std::cin, line);)
		inputJson += line;

	fromString(inputJson, account);

	std::cout << validate(account);
	std::cout << toString(account);

	/*
	Json::Value changes(Json::objectValue);
	changes["nameLast"] = "Fartface";
	changes["age"] = 34;
	changes["address"] = Json::Value(Json::objectValue);
	changes["address"]["city"] = "Hendertucky";
	changes["foods"] = Json::Value(Json::arrayValue);
	changes["foods"].append(Json::Value(Json::objectValue));
	changes["foods"].append(Json::Value(Json::objectValue));
	changes["foods"].append(Json::Value(Json::objectValue));
	changes["foods"][1]["name"] = "Some Kind Crazy Food";
	changes["foods"][2]["foodGroup"]["name"] = "Crazy Food Group";
	std::cout << writer.write(changes);
	//JsonCppApplyDiff changeBinder(changes);
	//account.bindMembers(changeBinder);

	Json::Value root(Json::objectValue);
	nbdl::JsonWrite writeBinder(root);
	account.bindMembers(writeBinder);
	std::cout << writer.write(root);
	*/
}

/*
path("/client/1/property/5/account/27");

path(Client{}, 1, Property{}, 5, Account{}, 27);

key(NBDL_MEMBER(&Account::id), 27);

client.path(Account{}, 1)
	.get(
		nbdl::cb::Done([&](Account account) {  }),
		nbdl::cb::NotFound([&]() { }),
		nbdl::cb::Fail([&]() {  }) 
	);

client.path(Account{}, 1)
	.listen(
		nbdl::cb::Diff([&](nbdl::Diff<Account> accountDiff) {  }),
		//below is only for the initial handshake
		nbdl::cb::NotFound([&]() { }),
		nbdl::cb::Fail([&]() {  }) 
	);
*/
