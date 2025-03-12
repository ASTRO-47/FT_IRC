#pragma once

#include "server.hpp"
class Client;

class Channel{
	private:
	std::string name;
	std::string pass;
	std::string topic;
	std::map<Client*, bool> members;
	// std::set<Client *> invited;
	bool isInviteOnly;
	bool requiresPass;
	bool limitSet;
	size_t userLimit;
	bool topicSettable;
	bool topicSet;
	public:
	std::string & getTopicString();
	std::string & getPass();
	bool & getRequiresPass();
	bool isOperator(Client*);
	bool isMember(Client*);
	void setOperator(Client* ,bool);
	
	void setPass(const std::string&);
	void setTopic(const std::string&);
	void setRequiresPass(const bool&);
	void appendMember(Client *);
	void setInviteOnly(bool);
	bool & getInviteOnly();
	std::map<Client*, bool>& getMembers();
	Channel(std::string, Client*);
	~Channel();
	void setUserLimit(size_t newLimit);
	size_t & getUserLimit();
	bool & getLimitSet();
	void setLimitSet(bool flag);
	unsigned long getNumMembers();
	bool & getTopicFlag();
	bool & getTopic();
	void setTopic(bool);
	void setTopicFlag(bool);
	std::string & getChannelName();
	void removeMember(Client *);
	void broadcastToAllMembers(std::string);
	std::string print_members();
};