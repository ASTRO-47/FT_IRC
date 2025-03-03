#include "Channel.hpp"

Channel::Channel(std::string ChannelName, Client* Creator, char prefix){
	name = ChannelName.substr(1);
	this->members[Creator] = true;
	isInviteOnly = false;
	pass = "";
	requiresPass = false;
	limitSet = false;
	topicSettable = false;
	userLimit = -1; // idan antchecki wach machi negative 3ad nchof wach limit tsetta
	// setOperator(Creator, true);
	std::string msg = ":" + Creator->get_nick_name() + "!~" + Creator->get_hostname() + "@" + Creator->get_ip() +" JOIN " + prefix + name + "\n"; 
	// std::string msg2 = ":" + Creator->get_nick_name() + "!~" + Creator->get_hostname() + "@" + Creator->get_ip() +" JOIN " + ChannelName + " +t" + "\n"; 
	// setTopic("");
	//rpl topic 332 && RPL_NAMREPLY 353 && ENDOFNAMES 366
	// mode notification
	send(Creator->get_socket_fd(), msg.c_str(), msg.length(), 0);
}

void Channel::appendMember(Client *newMember){ // zid lprefix
	this->members[newMember] = false;
	// correct reply;
	std::string msg = ":" + newMember->get_nick_name() + "!~" + newMember->get_hostname() + "@" + newMember->get_ip() +" JOIN " + name + "\n"; 
	send(newMember->get_socket_fd(), msg.c_str(), msg.length(), 0);
	// sendiha lga3 lmembers bli joina 
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

std::map<Client*, bool>& Channel::getMembers(){
	return members;
}

void Channel::setInviteOnly(bool flag){
	isInviteOnly = flag;
}

void Channel::setUserLimit(size_t newLimit){
	limitSet = true;
	userLimit = newLimit;
}

size_t & Channel::getUserLimit(){
	static size_t defaultVal = 0;
	if (limitSet == false){
		puts("no limit was set");
		return defaultVal;
	}
	return userLimit;
}

bool& Channel::getLimitSet(){
	return limitSet;
}

void Channel::setLimitSet(bool flag){
	limitSet = flag;
}

unsigned long Channel::getNumMembers(){
	return members.size();
}

bool & Channel::getTopicFlag(){
	return topicSettable;
}

void Channel::setTopicFlag(bool flag){
	topicSettable = flag;
}