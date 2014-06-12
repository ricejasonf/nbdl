asdfasdfasdfMOOOOOOOOOOOO
#ifndef BINDER_H
#define BINDER_H

#include<vector>
#include"Entity.h"

class Binder
{
	protected:

	friend void Entity::bindRelation(Binder &);
	virtual void bind(const std::string, Entity &) = 0;
	virtual void bind(const std::string, std::vector<Entity> &) = 0;
	//todo bind members of primitive type if the ol' string vector thing doesn't cut it

	inline void initValue(Entity &entity, const std::string name, const std::string value) { entity.initValue(name, value); }
};

#endif
