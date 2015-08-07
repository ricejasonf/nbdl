#ifndef NBDL_MSGID_HPP
#define NBDL_MSGID_HPP

/*
 * TODO - make emscripten random seed generator
 */

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/functional/hash.hpp>

namespace nbdl {

class MsgId
{
	public:

	using Uuid = boost::uuids::uuid;
	using RandomGenerator = boost::uuids::random_generator;

	private:

	Uuid id;

	public:

	MsgId(RandomGenerator& gen) : id(gen()) {}

	Uuid getId() const { return id; }

	bool operator == (const MsgId& other) const
	{
		return (id == other.id);
	}
};

}//nbdl

namespace std {

template<>
struct hash<nbdl::MsgId>
{
	size_t operator () (const nbdl::MsgId& msgId) const
	{
		return boost::hash<nbdl::MsgId::Uuid>()(msgId.getId());
	}
};

}//std

#endif
