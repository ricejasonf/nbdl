#ifndef JSONWRITE_H
#define JSONWRITE_H

#include<string>
#include<jsoncpp/json/json.h>

namespace nbdl {

class JsonWrite
{
	Json::Value &jsonVal;

	public:

	JsonWrite(Json::Value &value) :
		jsonVal(value)
	{}

	template<typename T>
	void bindMember(const std::string name, T & field)
	{ 
		jsonVal[name] = field; 
	}

	template<class BinderFn>
	void bindEntity(const std::string name, BinderFn bind)
	{
		auto obj = Json::Value(Json::objectValue);
		JsonWrite writer(obj);
		bind(writer);
		jsonVal[name] = obj;
	}

};

}//nbdl

#endif
