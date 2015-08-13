#ifndef NBDL_CONTEXT_HPP
#define NBDL_CONTEXT_HPP

namespace nbdl {

template<typename Server, typename Api>
class Context
{
	Server server;

	public:

	template<typename Path, typename... MatchFns>
	void read(Path path, MatchFns... fns)
	{
		Api::read(server, path, fns...);
	}
};

}//nbdl

#endif
