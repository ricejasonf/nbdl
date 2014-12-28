#include "ValidationBinder.hpp"
#include "Entity.hpp"

void ValidationBinder::validateChild(Entity &entity, Binder &e)
{
	entity.runValidation(e);
	entity.bindMembers(e);
	hasErrors_ = hasErrors_ || e.hasErrors();
}
