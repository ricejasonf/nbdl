#ifndef JSONREAD_H
#define JSONREAD_H

#include"Entity.h"
namespace Json { class Value; }


class JsonRead : public Entity::Binder
{
	public:

	JsonRead(Json::Value &value);

	static void fromString(std::string &json, Entity &entity);

	void bind(const std::string, bool &);
	void bind(const std::string, unsigned int &);
	void bind(const std::string, int &);
	void bind(const std::string, double &);
	void bind(const std::string, std::string &);
	void bind(const std::string name, Entity &entity);
	void bind(const std::string name, EntityListBase &list);
			
	private: 
	
	const Json::Value &jsonVal;
		
};

#endif
