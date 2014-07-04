#ifndef CHATNOTIFICATION_H
#define CHATNOTIFICATION_H

class ChatNotification : public Entity
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
	unsigned _message;
}
