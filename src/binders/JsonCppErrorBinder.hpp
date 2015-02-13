#ifndef NBDL_BINDERS_JSONCPPVALIDATIONERRORS_HPP
#define NBDL_BINDERS_JSONCPPVALIDATIONERRORS_HPP

#include<jsoncpp/json/json.h>

namespace nbdl {
namespace binders {

class JsonCppValidationErrors
{
	Json::Value &jsonVal;

	public: 

	using NameFormat = DefaultNameFormat;
	
	JsonCppValidationErrors(Json::Value &value)
		: jsonVal(value) {}

	void addError(std::string name, ErrorToken token)
	{
		auto array = Json::Value(Json::arrayValue);
		array.append(toString(token));
		jsonVal[name] = array;
	}

	JsonCppValidationErrors createChild(const std::string name)
	{
		auto obj = Json::Value(Json::objectValue);
		JsonCppValidationErrors error(obj);
		jsonVal[name] = obj;
		return error;
	}
};

}//binders
}//nbdl
#endif
