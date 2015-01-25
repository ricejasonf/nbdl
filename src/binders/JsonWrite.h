#ifndef JSONWRITE_H
#define JSONWRITE_H

#include<string>
#include<jsoncpp/json/json.h>
#include "../Binder.hpp"

namespace nbdl {

class JsonWrite : public Binder<JsonWrite>
{
	Json::Value &jsonVal;

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
	JsonWrite getSubBinder(const std::string name, Entity &entity)
	{
		auto obj = Json::Value(Json::objectValue);
		JsonWrite writer(obj);
		return writer;
	}
		
};

}//nbdl

#endif
