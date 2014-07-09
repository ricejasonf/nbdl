#ifndef CHATROOM_H
#define CHATROOM_H

class ChatRoom : public EntityPersistent
{
	public:
	
	void bindMembers(Binder&);
	void validate(ErrorBinder&);

	std::string name() { return _name; }
	ChatRoom& name(std::string v) { set(v, _name); return *this; }

	private:

	std::string _name;
};

class ChatRoomPath : public PathNode<ChatRoom>
{
	public:
	
	ChatRoomPath(unsigned id) : PathNode<ChatRoom>(id) {}

	ChatMessagePath messages(unsigned id = 0) { return ChatMessagePath(*this, id); }

	private:

	//multi index of messages
	boost::multi_index_container<
		ChatMessage,
		boost::multi_index::indexed_by<
			boost::multi_index::ordered_unique<
				boost::multi_index::member<ChatMessage, unsigned, &ChatMessage::_id>>,
			boost::multi_index::ordered_non_unique<
				boost::multi_index::member<ChatMessage, unsigned, &ChatMessage::_parentId>>
		>> _messages;
};
