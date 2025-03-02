#pragma once

#include "client.hpp"

#define RPL_TOPIC 

class Channel{
	private:
	std::string name;
	std::string pass;
	std::string topic;
	std::map<Client*, bool> members;
	bool isInviteOnly;
	bool requiresPass;
	public:
	std::string & getTopic();
	std::string & getPass();
	bool & getRequiresPass();
	bool isOperator(Client*);
	void setOperator(Client* ,bool);
	
	void setPass(const std::string&);
	void setTopic(const std::string&);
	void setRequiresPass(const bool&);

	Channel(std::string, Client*, char);
	~Channel();
};