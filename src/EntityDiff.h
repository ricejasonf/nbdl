#ifndef ENTITYDIFF
#define ENTITYDIFF

class EntityDiff
{
	public:

	template<typename TYPE>
	bool isDirty(TYPE &field, Entity *container)
	{
		return std::find(
				dirtyFields.begin(), 
				dirtyFields.end(), 
				&field - container) != dirtyFields.end();	
	}

	template<typename TYPE>
	void set(TYPE value, TYPE& field, Entity *container)
	{
		field = value;
		dirtyFields.push_back(&field - container)
	}

	private:

	std::vector<unsigned int> dirtyFields;
}

#endif
