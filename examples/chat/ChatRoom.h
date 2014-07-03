#ifndef CHATROOM_H
#define CHATROOM_H

class ChatRoom : public Entity
{
	public:
	
	void bindMembers(Binder&);
	void validate(ErrorBinder&);

	EntityList<ChatMessage> messages;

	unsigned id() { return _id; }
	unsigned name() { return _name; }

	ChatRoom& id(unsigned v) { set(v, _id); return *this; }
	ChatRoom& name(unsigned v) { set(v, _name); return *this; }

	private:

	unsigned _id;
	unsigned _name;
}
