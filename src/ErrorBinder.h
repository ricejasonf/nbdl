#ifndef ERRORBINDER_H
#define ERRORBINDER_H

class ErrorBinder : public Binder
{

	ErrorBinder() : _hasErrors(false) {}

	template<typename T>
	void addError(Entity &entity, T &field, std::string error)
	{
		_hasErrors = true;
		errors[&field - &entity].push_back(error);
	}
	bool hasErrors() { return _hasErrors; }

	protected:

	std::vector<std::string> getErrors(Entity &entity, T &field);

	void validateChild(Entity &entity, ErrorBinder &e)
	{
		entity.validate(e);
		entity.bindMembers(e);
		_hasErrors = _hasErrors || e.hasErrors();
	}

	private:

	bool _hasErrors;
	std::unordered_map<unsigned int, std::vector<<std::string>> errors;
}

#endif
