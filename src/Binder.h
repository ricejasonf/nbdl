#ifndef BINDER_H
#define BINDER_H

class Entity;
class EntityListBase;

template<class Derived>
class Binder
{
	bool _diffMode;

	public:

	//todo move diffMode stuff out of base Binder class
	Binder(bool diffMode = false) : _diffMode(diffMode) {}
	virtual ~Binder() {}
	void setDiffMode(bool b = true) { _diffMode = b; }

	bool diffMode() { return _diffMode; }

	//if EntityType is derived from Entity use that
   	//to prevent code bloat. otherwise allow ad hoc types
	template<class EntityType, typename T>
	typename std::enable_if<!std::is_base_of<Entity, EntityType>::value>::type
		bindMember(EntityType &entity, const std::string name, T &field)
	{
		static_cast<Derived*>(this)->bind(entity, name, field);
	}
	template<typename T>
	void bindMember(Entity &entity, const std::string name, T &field)
	{
		static_cast<Derived*>(this)->bind(entity, name, field);
	}

	protected:

	//implemented in Entity.hpp because 
	//it calls members of Entity (for convenience)
	template<typename T>
	void set(Entity &e, T value, T &field);

};

#endif
