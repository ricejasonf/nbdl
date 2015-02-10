#ifndef NBDL_VALIDATE_ERRORS_HPP
#define NBDL_VALIDATE_ERRORS_HPP

namespace nbdl {
namespace errors {

	//todo provide an int and string here maybe?
	struct Required {};
	struct TooLong {};
	struct TooShort {};
	struct MatchFail {};

}//errors
}//nbdl

#endif
