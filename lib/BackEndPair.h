#ifndef BACKENDPAIR_H
#define BACKENDPAIR_H

/*
 * this is my workaround for not using pointers to BackEnd all over the place
 */
template<class T1, class T2>
class BackEndPair : public BackEnd
{
	private:

	T1 b1;
	T2 b2;

	public:

	BackEndPair(T1 b1, T2 b2) :
		b1(b1),
		b1(b2) 
	{ 
		static_assert(std::is_base_of(BackEnd, T1)::value, "T1 must be derived from BackEnd");
		static_assert(std::is_base_of(BackEnd, T2)::value, "T2 must be derived from BackEnd");
	}

	void validate(Entity &entity) 
	{
		b1.validate(entity);
		b2.validate(entity);
	}

	void insert(const ValueMap values);
	{
		b1.insert(values);
		b1.insert(values);
	}
	void update(const ValueMap values);
	{
		b1.update(values);
		b1.update(values);
	}

	void beginTransaction()
	{
		b1.beginTransaction();
		b2.beginTransaction();
	}
	void commit()
	{
		b1.commit();
		b2.commit();
	}
	void rollback()
	{
		b1.rollback();
		b2.rollback();
	}
}

#endif
