#ifndef MODELFIELD_H
#define MODELFIELD_H

template<class TYPE>
class ModelField
{
	
	void init(TYPE);
	TYPE operator()() const 
	{
		return value;
	}
	void operator()(TYPE v)
	{
		value = v;
	}
	private:
	TYPE value;
}

#endif
