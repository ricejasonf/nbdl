#ifndef CHATMESSAGE_H
#define CHATMESSAGE_H

class ChatMessage : public Entity
{
	public:
	
	void bindMembers(Binder&);
	void validate(ErrorBinder&);

	EntityList<ChatMessage> replies;

	unsigned id() { return _id; }
	unsigned message() { return _message; }

	ChatMessage& id(unsigned v) { set(v, _id); return *this; }
	ChatMessage& message(unsigned v) { set(v, _message); return *this; }

	private:

	unsigned _id;
	unsigned _message;
	unsigned _mood;
}
