#include <memory>

/*
#include "../../lib/Entity.h"
#include "../../lib/BackEndPrint.h"
*/
#include <Entity.h>
#include <BackEndPrint.h>


Entity buildAccount() 
{ 
	return Entity(std::unique_ptr<BackEnd>(new BackEndPrint("Account"))); 
}

int main()
{
	Entity account = buildAccount();
//	Entity account(std::unique_ptr<BackEnd>(new BackEndPrint("Account")));
	account.set("NameFirst", "Joe");
	account.set("NameLast", "Smith");
	account.set("username", "joe");
	account.set("password", "iamasecret123");
	account.save();
}
