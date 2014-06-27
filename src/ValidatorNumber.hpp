template<typename T>
inline ValidatorNumber &
	ValidatorNumber<T>::max(const T v)
{
	if (field > v) addError("tooBig");
	return *this;
}

template<typename T>
inline ValidatorNumber &
	ValidatorNumber<T>::min(const T v)
{
	if (field < v) addError("tooSmall");
	return *this;
}
