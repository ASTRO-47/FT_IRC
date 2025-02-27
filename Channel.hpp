#pragma once

#include "client.hpp"

#define RPL_TOPIC 

class Channel{
	private:
	std::string name;
	std::string pass;
	std::string topic;
	std::map<Client*, bool> operators;
	bool isInviteOnly;
	public:
	std::string & getTopic();
	std::string & getPass();
	void setPass(const std::string&);
	void setTopic(const std::string&);

	Channel(std::string, Client*, char);
	~Channel();
};