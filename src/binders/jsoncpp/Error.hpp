#ifndef NBDL_BINDERS_JSONCPP_VALIDATION_ERRORS_HPP
#define NBDL_BINDERS_JSONCPP_VALIDATION_ERRORS_HPP

#include<jsoncpp/json/json.h>

namespace nbdl {
namespace binders {
namespace jsoncpp {

class ValidationErrors
{
	Json::Value &jsonVal;

	public: 

	using NameFormat = DefaultNameFormat;
	
	ValidationErrors(Json::Value &value)
		: jsonVal(value) {}

	void addError(std::string name, ErrorToken token)
	{
		jsonVal[name] = toString(token);
	}

	template<typename Functor>
	void validateChild(const std::string name, Functor fn)
	{
		Json::Value obj = Json::Value(Json::objectValue);
		ValidationErrors errors(obj);
		fn(errors);
		jsonVal[name] = obj;
	}
};

}//jsoncpp
}//binders
}//nbdl
#endif
