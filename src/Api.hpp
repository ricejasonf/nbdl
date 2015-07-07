#ifndef NBDL_API_HPP
#define NBDL_API_HPP

namespace nbdl {

template<class Server, class ApiImpl>
class Api
{
	Server server; 
	MsgHandler msgHandler;

	public:
	//todo server
	//todo message id generator
	template<typename Path>
	Promise<Path::Entity> read(Path path)
	{
		//create msg with callback
		//send to server
		callback = ApiImpl::read(server, path);
	}
};

}//nbdl

#endif
