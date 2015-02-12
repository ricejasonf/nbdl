#ifndef NBDL_ERRORTOKEN_HPP
#define NBDL_ERRORTOKEN_HPP

namespace nbdl {

	enum struct ErrorToken
	{
		Invalid,
		Required,
		TooLong,
		TooShort,
		AboveMax,
		BelowMin,
		MatchFail
	}

}//nbdl

#endif
