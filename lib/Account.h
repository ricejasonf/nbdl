#ifndef ACCOUNT_H
#define ACCOUNT_H

using namespace std::string;

Account *account = new Account();
	account->setNameFirst("Joe");
	account->setNameLast("Higashi");
	account->setUsername("kicky1995");
	account->setPassword("moomoo1234");
	account->create();


class Account
{
	private:
		
	public:
		void setNameFirst(string);
		void setNameLast(string);
		void setUsername(string);
		void setPassword(string);
		void setAge(string);
		void setAge(int);
}

#endif
