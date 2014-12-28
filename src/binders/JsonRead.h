#ifndef JSONREAD_H
#define JSONREAD_H

#include "../Binder.hpp"

namespace Json { class Value; }

class JsonRead : public Binder<JsonRead>
{
	friend class Binder<JsonRead>;
	void bind(const std::string, bool &);
	void bind(const std::string, unsigned int &);
	void bind(const std::string, int &);
	void bind(const std::string, double &);
	void bind(const std::string, std::string &);

	template<class EntityType>
	void bindEntity(const std::string name, EntityType &entity)
	{
		//i'm not sure if this is where you would use std::move
		entity.bindMembers(createObjectReader(name));
	}
	JsonRead createObjectReader(const std::string name);

	const Json::Value &jsonVal;

	public:

	JsonRead(const Json::Value &value, bool diffMode = false);

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

#endif
