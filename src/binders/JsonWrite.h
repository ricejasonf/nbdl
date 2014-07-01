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

	void bind(Entity &parent, const std::string name, bool & field);
	void bind(Entity &parent, const std::string name, unsigned int & field);
	void bind(Entity &parent, const std::string name, int & field);
	void bind(Entity &parent, const std::string name, double & field);
	void bind(Entity &parent, const std::string name, std::string & field);
	void bind(Entity &parent, const std::string name, Entity &entity);
	void bind(Entity &parent, const std::string name, EntityListBase &list);
			
	private: 
	
	Json::Value &jsonVal;
		
};

#endif
