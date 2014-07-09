#ifndef CHATMESSAGE_H
#define CHATMESSAGE_H

class ChatMessage : public Entity
{
	public:
	
	void bindMembers(Binder&);
	void validate(ErrorBinder&);

	unsigned id() { return _id; }
	unsigned message() { return _message; }

	ChatMessage& id(unsigned v) { set(v, _id); return *this; }
	ChatMessage& message(unsigned v) { set(v, _message); return *this; }

	private:

	unsigned _id;
	unsigned _parentMessageId;
	std::string _message;
	unsigned _mood;
}

class ChatMessagePath : public PathNode<ChatMessage>
{
	public:

	ChatMessagePath(ChatRoomPath room, unsigned id) : 
		room(room),
		PathNode<ChatMessage>(id) {}

	std::string getPath() { return getPath(room); }

	private:

	ChatRoomPath room;

}
