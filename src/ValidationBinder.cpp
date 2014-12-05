#include "ValidationBinder.hpp"
#include "Entity.hpp"

void ValidationBinder::validateChild(Entity &entity, ValidationBinder &e)
{
	entity.runValidation(e);
	entity.bindMembers(e);
	hasErrors_ = hasErrors_ || e.hasErrors();
}
