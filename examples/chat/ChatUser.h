#ifndef CHATUSER_H
#define CHATUSER_H

class ChatUser : public Entity
{
	public:
	
	void bindMembers(Binder&);
	void validate(ErrorBinder&);

	EntityList<ChatNotification> notifications;

	private:

	unsigned _id;
	unsigned _mood;
	std::string _username;
	std::string _realName;

}
