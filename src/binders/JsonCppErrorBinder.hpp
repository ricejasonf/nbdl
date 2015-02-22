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
		jsonVal[name] = toString(token);
	}

	template<typename Functor>
	void validateChild(const std::string name, Functor fn)
	{
		Json::Value obj = Json::Value(Json::objectValue);
		JsonCppValidationErrors errors(obj);
		fn(errors);
		jsonVal[name] = obj;
	}
};

}//binders
}//nbdl
#endif
