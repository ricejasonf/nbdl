#ifndef JSONWRITE_H
#define JSONWRITE_H

#include<string>
#include<jsoncpp/json/json.h>
#include "../Binder.hpp"

class JsonWrite : public Binder<JsonWrite>
{
	public:

	JsonWrite(Json::Value &value) :
		jsonVal(value), 
		Binder() {}

	template<class Entity>
	static std::string toString(Entity &entity)
	{
		Json::StyledWriter writer;
		Json::Value root;
		JsonWrite r(root);
		entity.bindMembers(r);
		return writer.write(root);
	}

	template<typename T>
	void bind(const std::string name, T & field)
	{ 
		jsonVal[name] = field; 
	}

	template<class Entity>
	void bindEntity(const std::string name, Entity &entity)
	{
		auto obj = Json::Value(Json::objectValue);
		JsonWrite writer(obj);
		entity.bindMembers(writer);
		if (obj.size())
			jsonVal[name] = obj;
	}
			
	private: 
	
	Json::Value &jsonVal;
		
};

#endif
