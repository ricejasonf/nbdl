#ifndef NBDL_CONTEXT_HPP
#define NBDL_CONTEXT_HPP

namepace nbdl {

template<typename Server, typename Api>
class Context
{
	Server conn;
	Api api;

	public:
	//todo server
	//todo message id generator
	template<typename Path>
	Promise<Path::Entity> read(Path path)
	{
		//create msg with callback
		//send to server
		callback = Api::read(server, path);
	}
};

}//nbdl

#endif
