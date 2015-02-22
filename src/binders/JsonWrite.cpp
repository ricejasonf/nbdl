#include<stdexcept>
#include "JsonWrite.h"

namespace nbdl {

//todo replace this stuff with a template
void JsonWrite::bind(const std::string name, bool & field) 
{ 
	jsonVal[name] = field; 
}
void JsonWrite::bind(const std::string name, unsigned int & field) 
{ 
	jsonVal[name] = field; 
}
void JsonWrite::bind(const std::string name, int & field) 
{ 
	jsonVal[name] = field; 
}
void JsonWrite::bind(const std::string name, double & field) 
{ 
	jsonVal[name] = field; 
}
void JsonWrite::bind(const std::string name, std::string & field) 
{ 
	jsonVal[name] = field; 
}

}//nbdl
