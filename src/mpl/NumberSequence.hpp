#ifndef NBDL_MPL_NUMBER_SEQUENCE
#define NBDL_MPL_NUMBER_SEQUENCE

namespace nbdl {
namespace mpl {

template<int... sequence>
struct NumberSequence {};

template<int n, int... sequence>
struct CreateNumberSequence
{
	using Type = typename GenerateNumberSequence<n - 1, n - 1, sequence...>::Type;
};
template<int... sequence>
struct CreateNumberSequence<0, sequence...>
{
	using Type = NumberSequence<sequence...>;
};

}//mpl
}//nbdl

#endif
