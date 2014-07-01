#ifndef JSONREAD_H
#define JSONREAD_H

#include "../Binder.h"

namespace Json { class Value; }

class JsonRead : public Binder
{
	public:

	JsonRead(const Json::Value &value, bool diffMode = false);

	static void fromString(std::string &json, Entity &entity);

	void bind(Entity &parent, const std::string, bool &);
	void bind(Entity &parent, const std::string, unsigned int &);
	void bind(Entity &parent, const std::string, int &);
	void bind(Entity &parent, const std::string, double &);
	void bind(Entity &parent, const std::string, std::string &);
	void bind(Entity &parent, const std::string name, Entity &entity);
	void bind(Entity &parent, const std::string name, EntityListBase &list);
			
	private: 
	
	const Json::Value &jsonVal;
		
};

#endif
