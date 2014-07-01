template<typename T>
inline ValidatorNumber<T> &
	ValidatorNumber<T>::max(const T v)
{
	if (this->field > v) this->addError("tooBig");
	return *this;
}

template<typename T>
inline ValidatorNumber<T> &
	ValidatorNumber<T>::min(const T v)
{
	if (this->field < v) this->addError("tooSmall");
	return *this;
}
