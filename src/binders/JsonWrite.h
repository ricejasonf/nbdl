#ifndef JSONWRITE_H
#define JSONWRITE_H

class Entity;
class EntityListBase;

class JsonWrite : public Binder
{
	public:

	JsonWrite(Json::Value &value, bool diffMode = false) :
		jsonVal(value), 
		Binder(diffMode) {}

	static std::string toString(Entity &entity);

	template<typename T>
	void bind(Entity &parent, const std::string name, T &value) 
	{ 
		if (isDiffMode() && !parent.isDirty(value))
			return;
		jsonVal[name] = value; 
	}

	void bind(Entity &parent, const std::string name, Entity &entity);
	void bind(Entity &parent, const std::string name, EntityListBase &list);
			
	private: 
	
	Json::Value &jsonVal;
		
};

#endif
