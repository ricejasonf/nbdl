#ifndef JSONREAD_H
#define JSONREAD_H

#include<string>
#include<jsoncpp/json/json.h>

namespace nbdl {

class JsonRead
{
	void bind(const std::string, bool &);
	void bind(const std::string, unsigned int &);
	void bind(const std::string, int &);
	void bind(const std::string, double &);
	void bind(const std::string, std::string &);

	JsonRead createObjectReader(const std::string name);

	const Json::Value &jsonVal;

	public:

	template<typename T>
	void bindMember(const std::string name, T &field)
	{
		bind(name, field);
	}

	template<class BinderFn>
	void bindEntity(const std::string name, BinderFn bindFn)
	{
		JsonRead reader = createObjectReader(name);
		bindFn(reader);
	}

	JsonRead(const Json::Value &value);

	template<class EntityType>
	static void fromString(std::string &json, EntityType &entity)
	{
		Json::Reader reader;
		Json::Value root;
		if (!reader.parse(json, root, false))
		{
			throw std::runtime_error("JSON parse error");
		}
		JsonRead r(root);
		entity.bindMembers(r);
	}
			
};

}//nbdl

#endif
