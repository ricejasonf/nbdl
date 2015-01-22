#ifndef JSONCPPAPPLYDIFF_HPP
#define JSONCPPAPPLYDIFF_HPP

#include<string>
#include<jsoncpp/json/json.h>
#include "../Binder.hpp"

namespace nbdl {

class JsonCppApplyDiff : public Binder<JsonCppApplyDiff>
{
	friend class Binder<JsonCppApplyDiff>;
	void bind(const std::string, bool &);
	void bind(const std::string, unsigned int &);
	void bind(const std::string, int &);
	void bind(const std::string, double &);
	void bind(const std::string, std::string &);

	template<class EntityType>
	void bindEntity(const std::string name, EntityType &entity)
	{
		if (!jsonVal.isMember(name))
			return;
		JsonCppApplyDiff reader = createObjectReader(name);
		entity.bindMembers(reader);
	}

	JsonCppApplyDiff createObjectReader(const std::string name);

	const Json::Value &jsonVal;

	public:

	JsonCppApplyDiff(const Json::Value &value);

	template<class EntityType>
	static void fromString(std::string &json, EntityType &entity)
	{
		Json::Reader reader;
		Json::Value root;
		if (!reader.parse(json, root, false))
		{
			throw std::runtime_error("JSON parse error");
		}
		JsonCppApplyDiff r(root);
		entity.bindMembers(r);
	}
			
};

}//nbdl

#endif
