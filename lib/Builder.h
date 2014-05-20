#ifndef BUILDER_H
#define BUILDER_H

#include "Entity.h"
#include "BackEndPrint.h"

class Builder
{
	public:

	Account buildAccount() { return Account(*this, unique_ptr<BackEnd>(new BackEndPrint("Account")); }

}

#endif
