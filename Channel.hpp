#pragma once

#include "client.hpp"

#define RPL_TOPIC 

class Channel{
	private:
	std::string name;
	std::string topic;
	std::map<Client*, bool> operators;
	bool isInviteOnly;
	public:
	std::string & getTopic();
	void setTopic(const std::string&);
	Channel(std::string, Client*);
	~Channel();
};