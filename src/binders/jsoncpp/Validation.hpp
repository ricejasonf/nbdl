#ifndef NBDL_BINDERS_JSONCPP_VALIDATION_HPP
#define NBDL_BINDERS_JSONCPP_VALIDATION_HPP
//DEPRECATED or needs refactoring

#include "ValidationBinder.hpp"

namespace nbdl {
namespace binders {
namespace jsoncpp {

class Entity;
class EntityListBase;

class Validation : public ValidationBinder<Validation>
{
	public:

	Validation(Json::Value &value) :
		jsonVal(value) {}

	template<typename T>
	void bind(Entity &parent, const std::string name, T &value);
	{ 
		writeErrors(getErrors(parent, value));
	}

	//todo needs sfinae for bases of Entity
	void bind(Entity &parent, const std::string name, Entity &entity);
	void bind(Entity &parent, const std::string name, EntityListBase &list);
			
	private: 

	void writeErrors(std::string name, std::vector<std::string> errors);
	void validate(Entity &, JsonError);
	
	Json::Value &jsonVal;
		
};

}//jsoncpp
}//binders
}//nbdl

#endif
