#include "../../lib/Entity.h"
#include "../../lib/BackEndPrint.h"

//Entity buildAccount() { return Entity(unique_ptr<BackEnd>(new BackEndPrint("Account"))); }

int main()
{
	//Entity account = buildAccount();
	Entity account(unique_ptr<BackEnd>(new BackEndPrint("Account")));
	account.set("NameFirst", "Joe");
	account.set("NameLast", "Smith");
	account.set("username", "joe");
	account.set("password", "iamasecret123");
	account.save();
}
