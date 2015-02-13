#ifndef NBDL_ERRORTOKEN_HPP
#define NBDL_ERRORTOKEN_HPP

namespace nbdl {

//todo make macro to build this
enum struct ErrorToken
{
	Invalid,
	Required,
	TooLong,
	TooShort,
	AboveMax,
	BelowMin,
	MatchFail,
	Custom1 = 1000,
	Custom2,
	Custom3,
	Custom4,
	Custom5,
	Custom6,
	Custom7,
	Custom8,
	Custom9
};

static constexpr const char *toString(ErrorToken t)
{
	return	t == ErrorToken::Invalid	? "Invalid" 	:
			t == ErrorToken::Required	? "Required" 	:
			t == ErrorToken::TooLong	? "TooLong" 	:
			t == ErrorToken::TooShort	? "TooShort" 	:
			t == ErrorToken::AboveMax	? "AboveMax" 	:
			t == ErrorToken::BelowMin	? "BelowMin" 	:
			t == ErrorToken::MatchFail	? "MatchFail" 	:
			t == ErrorToken::Custom1	? "Custom1" 	:
			t == ErrorToken::Custom2	? "Custom2" 	:
			t == ErrorToken::Custom3	? "Custom3" 	:
			t == ErrorToken::Custom4	? "Custom4" 	:
			t == ErrorToken::Custom5	? "Custom5" 	:
			t == ErrorToken::Custom6	? "Custom6" 	:
			t == ErrorToken::Custom7	? "Custom7" 	:
			t == ErrorToken::Custom8	? "Custom8" 	:
			t == ErrorToken::Custom9	? "Custom9" 	:
			"Invalid";
}

}//nbdl

#endif
