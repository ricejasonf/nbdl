#include <iostream>
#include <string>

#include <BackEndPrint.h>
#include <binders/JsonRead.h>
#include <binders/JsonWrite.h>
#include "Account.h"

Account buildAccount() 
{ 
	return Account(BackEnd::Ptr(new BackEndPrint("Account"))); 
}

int main()
{
	Account account = buildAccount();
	std::string inputJson, outputJson;
	for (std::string line; std::getline(std::cin, line);)
		inputJson += line;
	JsonRead::fromString(inputJson, account);
	std::cout << JsonWrite::toString(account);

	MyArcusEntityRoot arcus();
	/* 
	   the arcusContainer should have the backend pointer and the entity root creates
	   request objects with callbacks
	 */
	arcusContainer.async(arcus.clients(15).accounts(5)
			.retrieve()
				.success([](Account &a) { /*do something with the account*/ })
				.notFound([]() { /*do something else*/ })
				.fail([]() { /*oh crap*/ })
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
				.success([]() { /*do something with the account*/ })
				.validationFail([]() { /*do something else*/ })
				.fail([]() { /*oh crap*/ })
		);

	arcusContainer.async(arcus.clients(15).accounts(5)
			.listen([this](ValueMap diff) { /* blah */ })
		);
			
	/*
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
	*/
}
