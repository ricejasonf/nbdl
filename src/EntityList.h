#ifndef ENTITYLIST_H
#define ENTITYLIST_H

#include<type_traits>

class EntityListBase
{
	public:

	typedef std::vector<std::reference_wrapper<Entity>> EntityRefs;
	typedef std::vector::size_type size_type;

	EntityRefs getEntityRefs() = 0;
	Entity &appendNewEntity() = 0;
	void resize(size_type) = 0;
	size_type size() = 0;
}

template<typename T>
class EntityList : public EntityListBase
{
	static_assert(std::is_base_of(Entity, T)::value, "T must be derived from Entity");

	public:

	typedef std::vector<std::reference_wrapper<Entity&>> EntityRefs;

	Entity &addNew()
	{
		//some kind of dependency injection stuff needs to go here
		T *entity = new T();
		vec.push_back(entity);
		return *entity;
	}

	size_type size()
	{
		return vec.size();
	}

	protected:

	EntityRefs getEntityRefs()
	{
		return EntityRefs(vec.begin(), vec.end());
	}

	void resize(size_type n)
	{
		vec.resize(n);
	}


	private:

	std::vector<Entity::Ptr> vec;
}

#endif
