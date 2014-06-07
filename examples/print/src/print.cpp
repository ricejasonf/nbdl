//#include <memory>
#include <iostream>

#include <BackEndPrint.h>
#include "Account.h"

Account buildAccount() 
{ 
	return Account(std::unique_ptr<BackEnd>(new BackEndPrint("Account"))); 
}

int main()
{
	std::cout << "hello account" << "\n";
	Account account = buildAccount();

	account.createAddress()
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
