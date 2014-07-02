#include <iostream>
#include <string>

#include <binders/JsonRead.h>
#include <binders/JsonWrite.h>
#include <jsoncpp/json/json.h>
#include "Account.h"

int main()
{
	auto account = Account();
	std::string inputJson, outputJson;
	for (std::string line; std::getline(std::cin, line);)
		inputJson += line;

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

	JsonRead::fromString(inputJson, account);

	std::cout << JsonWrite::toString(account);

	JsonRead changeBinder(changes, true);
	account.bindMembers(changeBinder);

	Json::StyledWriter writer;
	auto root = Json::Value(Json::objectValue);
	JsonWrite writeBinder(root, true);
	account.bindMembers(writeBinder);
	std::cout << writer.write(root);

	std::cout << JsonWrite::toString(account);

}

	//MyArcusEntityRoot arcus();
	/*
	   the arcusContainer should have the backend pointer and the entity root creates
	   request objects with callbacks
	arcusContainer.async(arcus.clients(15).accounts(5)
			.retrieve()
				.success([](Account &a) { })
				.notFound([]() {  })
				.fail([]() {  })
		);
	
	auto account = arcus.clients(15).accounts(5).create();
	account
		.nameFirst("Blah")
		.age(15)
		.address(Address().line1("mooo"))
		.address()
			.line1("123 Spring Rd.")
			.city("Henderson")
			.zipCode(89015)
		.addRun(Run()
			.seconds(1045)
			.miles(1.65))
		;
	arcusContainer.async(arcus
			.save(account)
				.success([]() {  })
				.validationFail([]() {  })
				.fail([]() {  })
		);

	arcusContainer.async(arcus.clients(15).accounts(5)
			.listen([this](ValueMap diff) {  })
		);
			
	account.getAddress()
		.setLine1("123 Spork Rd.")
		.setCity("Henderson")
		.setState("NV")
		.setZip("89015");

	if (!account
		.setNameFirst("Jow")
		.setNameLast("Smart")
		.setPhoneNumber("5553451234")
		.save())
	{
		std::cout << "Validation failed with the following errors:" << std::endl;
		for (auto &i : account.getErrors())
		{
			std::cout << "\t" << i.first << std::endl;
			for (auto &j : i.second)
			{
				std::cout << "\t\t" << j << std::endl;
			}
		}
	}
}
	*/
