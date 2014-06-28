class Entity;
class EntityListBase;

class Binder
{
	public:

	Binder(bool diffMode = false) : _isDiffMode(diffMode) {}
	void setDiffMode(bool b = true) { _diffMode = b; return *this; }

	protected:
	friend class Entity;
	//first entity ref is the owner used only to
	//determine dirtyness when it is needed
	virtual void bind(Entity &, const std::string, bool &) = 0;
	virtual void bind(Entity &, const std::string, unsigned int &) = 0;
	virtual void bind(Entity &, const std::string, int &) = 0;
	virtual void bind(Entity &, const std::string, double &) = 0;
	virtual void bind(Entity &, const std::string, std::string &) = 0;
	virtual void bind(Entity &, const std::string, Entity &) = 0;
	virtual void bind(Entity &, const std::string, EntityListBase &) = 0;

	inline bool diffMode() { return _diffMode; }

	private:

	bool _diffMode;
};
