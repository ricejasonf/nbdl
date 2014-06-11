#ifndef BINDER_H
#define BINDER_H

class Binder
{
	protected:

	virtual void bindEntity(const std::string, Entity &) = 0;
	virtual void bindEntityList(const std::string, std::vector<Entity> &) = 0;

	//todo bind members of primitive type if the ol' string vector doesn't cut it
}

#endif
