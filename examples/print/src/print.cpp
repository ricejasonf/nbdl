#include <iostream>
#include <string>

#include <binders/JsonRead.h>
#include <binders/JsonWrite.h>
#include <binders/JsonCppApplyDiff.hpp>
#include <jsoncpp/json/json.h>

#include <EntityTraits.hpp>
#include <MemberSet.hpp>
#include <Bind.hpp>
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

int main()
{
	Json::StyledWriter writer;
	auto account = Account();
	std::string inputJson, outputJson;
	for (std::string line; std::getline(std::cin, line);)
		inputJson += line;

	fromString(inputJson, account);
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
	//MyArcusEntityRoot arcus();
	   the arcusContainer should have the backend pointer and the entity root creates
	   request objects with callbacks
	arcusContainer.async(arcus.clients(15).accounts(5)
			.retrieve()
				.success([](Account &a) { })
				.notFound([]() {  })
				.fail([]() {  })
		);
	
	Account account = arcus.clients(15).accounts(5).create();
	account
		.nameFirst("Blah")
		.age(15)
		.address()
			.line1("123 Spring Rd.")
			.city("Henderson")
			.zipCode(89015)
		.addRun(Run()
			.seconds(1045)
			.miles(1.65))
		;
	arcusContainer.async(arcus.clients(15).accounts(5)
			.save(account)
				.success([]() {  })
				.validationFail([]() {  })
				.fail([]() {  })
		);

	arcusContainer.async(arcus.clients(15).accounts(5)
			.listen([this](ValueMap diff) {  })
		);
			
	account.address()
		.setLine1("123 Spork Rd.")
		.setCity("Henderson")
		.setState("NV")
		.setZip("89015");

}
*/
