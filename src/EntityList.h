#ifndef ENTITYLIST_H
#define ENTITYLIST_H

#include<type_traits>

class Entity;

class EntityListBase
{
	public:

	virtual ~EntityListBase() {}

	typedef std::reference_wrapper<Entity> EntityRef;
	typedef std::vector<EntityRef> EntityRefs;
	typedef std::vector<Entity::Ptr>::size_type size_type;

	virtual EntityRefs getRefs() = 0;
	virtual Entity &appendNew() = 0;
	virtual void initWithSize(size_type) = 0;
	virtual size_type size() = 0;
};

template<typename T>
class EntityList : public EntityListBase
{
	static_assert(std::is_base_of<Entity, T>::value, "T must be derived from Entity");

	public:

	EntityList(BackEnd::Ptr backend) :
		backEnd(backEnd) {}

	Entity &appendNew()
	{
		T *entity = new T(backEnd);
		vec.push_back(Entity::Ptr(entity));
		return *entity;
	}

	EntityListBase::size_type size()
	{
		return vec.size();
	}

	EntityListBase::EntityRefs getRefs()
	{
		auto refs = EntityListBase::EntityRefs();
		for (auto &i: vec)
		{
			refs.push_back(*i);
		}
		return refs;
	}

	protected:

	void initWithSize(EntityListBase::size_type n)
	{
		vec.clear();
		for (int i = 0; i < n; i++)
			appendNew();
	}

	BackEnd::Ptr backEnd;

	private:

	std::vector<Entity::Ptr> vec;
};

#endif
