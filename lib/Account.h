#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "Entity.h"

class Account : public Entity
{
	public:
		Account &setNameFirst(string) { set("NameFirst"); return *this; }
		Account &setNameLast(string) { set("NameLast"); return *this; }
		Account &setUsername(string) { set("Username"); return *this; }
		Account &setPassword(string) { set("Password"); return *this; }
		Account &setAge(string) { set("Age"); return *this; }
}

#endif
