//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_BINDERS_JSONCPP_APPLY_DIFF_HPP
#define NBDL_BINDERS_JSONCPP_APPLY_DIFF_HPP

#include<string>
#include<jsoncpp/json/json.h>
#include "../../Binder.hpp"

//this needs refactoring
namespace nbdl {
namespace binders {
namespace jsoncpp {

class ApplyDiff : public Binder<ApplyDiff>
{
	friend class Binder<ApplyDiff>;
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
		ApplyDiff reader = createObjectReader(name);
		entity.bindMembers(reader);
	}

	ApplyDiff createObjectReader(const std::string name);

	const Json::Value &jsonVal;

	public:

	ApplyDiff(const Json::Value &value);

	template<class EntityType>
	static void fromString(std::string &json, EntityType &entity)
	{
		Json::Reader reader;
		Json::Value root;
		if (!reader.parse(json, root, false))
		{
			throw std::runtime_error("JSON parse error");
		}
		ApplyDiff r(root);
		entity.bindMembers(r);
	}
			
};

}//jsoncpp
}//binders
}//nbdl

#endif
