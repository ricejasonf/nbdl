#include <iostream>
#include <string>

#include <BackEndPrint.h>
#include <JsonUnserialize.h>
#include <JsonSerialize.h>
#include "Account.h"

Account buildAccount() 
{ 
	return Account(BackEnd::Ptr(new BackEndPrint("Account"))); 
}

int main()
{
	Account origAccount = buildAccount();
	Account account = std::move(origAccount);
	std::string inputJson, outputJson;
	for (std::string line; std::getline(std::cin, line);)
		inputJson += line;
	JsonUnserialize::fromString(inputJson, account);
	std::cout << JsonSerialize::toString(account);

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
