#ifndef NBDL_CONTEXT_HPP
#define NBDL_CONTEXT_HPP

namepace nbdl {

template<typename ServerConnection, typename Api>
class Context
{
	ServerConnection conn;
	Api api;

};

}//nbdl

#endif
