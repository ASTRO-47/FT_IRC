#include "Channel.hpp"

Channel::Channel(std::string ChannelName, Client* Creator, char prefix){
	name = ChannelName.substr(1);
	this->members[Creator] = true;
	isInviteOnly = false;
	pass = "";
	requiresPass = false;
	// setOperator(Creator, true);
	std::string msg = ":" + Creator->get_nick_name() + "!~" + Creator->get_hostname() + "@" + Creator->get_ip() +" JOIN " + prefix + name + "\n"; 
	// std::string msg2 = ":" + Creator->get_nick_name() + "!~" + Creator->get_hostname() + "@" + Creator->get_ip() +" JOIN " + ChannelName + " +t" + "\n"; 
	// setTopic("");
	//rpl topic 332 && RPL_NAMREPLY 353 && ENDOFNAMES 366
	// mode notification
	send(Creator->get_socket_fd(), msg.c_str(), msg.length(), 0);
}

std::string & Channel::getTopic(){
	return topic;
}

void Channel::setTopic(const std::string &newtopic){
	topic = newtopic;
}

std::string & Channel::getPass(){
	return pass;
}

void Channel::setPass(const std::string &newPass){
	pass = newPass;
}

Channel::~Channel(){
	
}

void Channel::setRequiresPass(const bool &NewStatus){
	requiresPass = NewStatus;
}

bool & Channel::getRequiresPass(){
	return requiresPass;
}

bool Channel::isOperator(Client* client){
	std::map<Client*,bool>::iterator it = members.find(client);
	if (it != members.end())
		return it->second;
	return false;
}

void Channel::setOperator(Client* client, bool flag){
	members[client] = flag;
}