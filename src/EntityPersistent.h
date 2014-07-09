#ifndef ENTITYPERSISTENT_H
#define ENTITYPERSISTENT_H

template<typename KEY = unsigned>
class EntityPersistent : public Entity
{
	public:

	virtual ~Entity() {}	

	bool isNew();

	KEY id() { return _id; }

	protected:

	//_id is protected to allow derived to bind it
	KEY _id;
};

//implementations 
template<>
bool
EntityPersistent<unsigned>::isNew()
{
	return _id > 0;
}
template<>
bool
EntityPersistent<std::string>::isNew()
{
	return _id.size() > 0;
}

#endif
