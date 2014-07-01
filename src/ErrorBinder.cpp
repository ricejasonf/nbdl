#include "ErrorBinder.h"
#include "Entity.h"

void ErrorBinder::validateChild(Entity &entity, ErrorBinder &e)
{
	entity.validate(e);
	entity.bindMembers(e);
	_hasErrors = _hasErrors || e.hasErrors();
}
