#ifndef NBDL_CONTEXT_HPP
#define NBDL_CONTEXT_HPP

namespace nbdl {

template<typename Server, typename Api>
class Context
{
	Server server;
	Api api;

	public:
/*
	template<typename Path, typename... MatchFns>
	void read(Path path, MatchFns...)
	{
		//create msg with callback
		//send to server
		MsgCreate<Path::Entity> msg;
		Api::read(server, path);
	}
	*/
};

}//nbdl

#endif
