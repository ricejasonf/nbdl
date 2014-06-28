#ifndef ERRORBINDER_H
#define ERRORBINDER_H

class ErrorBinder : public Binder
{
	template<typename T>
	void addError(Entity &entity, T &field, std::string error)
	{
		errors[&field - &entity].push_back(error);
	}

	protected:

	std::vector<std::string> getErrors(Entity &entity, T &field);

	private:

	std::unordered_map<unsigned int, std::vector<<std::string>> errors;
}

#endif
