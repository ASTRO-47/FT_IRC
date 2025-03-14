#include "Channel.hpp"

Channel::Channel(std::string ChannelName, Client* Creator){
	name = ChannelName;
	this->members[Creator] = true;
	isInviteOnly = false;
	pass = "";
	requiresPass = false;
	limitSet = false;
	topicSettable = false;
	topicSet = false;
	topic = "";
	userLimit = -1; // idan antchecki wach machi negative 3ad nchof wach limit tsetta
	Server::send_reply(Creator->get_socket_fd(), CHANNEL_JOIN(Creator->get_nick_name(), ChannelName, Creator->get_ip(), Creator->get_hostname()));
	Server::send_reply(Creator->get_socket_fd(), CHANNEL_MODES(Creator->get_nick_name(), ChannelName, Creator->get_ip(), Creator->get_hostname()));
	Server::send_reply(Creator->get_socket_fd(), RPL_NAMREPLY(Creator->get_nick_name(), ChannelName, '@' + Creator->get_nick_name()));
	Server::send_reply(Creator->get_socket_fd(), RPL_ENDOFNAMES(Creator->get_nick_name(), ChannelName));
}

void Channel::appendMember(Client *newMember){
	this->members[newMember] = false;
}

Channel::Channel() {}

std::string & Channel::getTopicString(){
	return topic;
}
void Channel::setTopicString(std::string &newTopic){
	topic = newTopic;
}

std::string & Channel::getChannelName(){
	return name;
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
	for (std::map<Client *, bool>::iterator it = members.begin(); it != members.end();){
		it = members.erase(it);
	}
}

void Channel::setRequiresPass(const bool &NewStatus){
	requiresPass = NewStatus;
}

bool & Channel::getRequiresPass(){
	return requiresPass;
}

void Channel::removeMember(Client *toRemove){
	if (members.find(toRemove) != members.end()){
		broadcastToAllMembers(CHANNEL_QUIT(toRemove->get_nick_name(), getChannelName(), toRemove->get_ip(), toRemove->get_hostname()));
		members.erase(toRemove);
	}
}

bool Channel::isMember(Client* client){
	std::map<Client*,bool>::iterator it = members.find(client);
	if (it != members.end())
		return true;
	return false;
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

bool & Channel::getInviteOnly(){
	return isInviteOnly;
}

void Channel::setUserLimit(size_t newLimit){
	limitSet = true;
	userLimit = newLimit;
}

size_t & Channel::getUserLimit(){
	static size_t defaultVal = 0;
	if (limitSet == false){
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

void Channel::broadcastToAllMembers(std::string msg){
	for (std::map<Client*, bool>::iterator it = members.begin(); it != members.end(); it++){
		if (it->first)
			Server::send_reply(it->first->get_socket_fd(), msg);
	}
}

std::string Channel::print_members(){
	std::stringstream ss;
	for (std::map<Client *, bool>::iterator it = members.begin(); it != members.end(); it++){
		if (it != members.begin())
			ss << " ";
		if (it->second == true)
			ss << "@";
		ss << it->first->get_nick_name();
	}
	return ss.str();
}

void Channel::setTopic(bool newState){
	topicSet = newState;
}

bool & Channel::getTopic(){
	return topicSet;
}

void Channel::appendInvitedMembers(Client *newInvited){
    invitedMembers.push_back(newInvited);
}

std::vector<Client*>& Channel::getInvitedMembers(){
    return invitedMembers;
}

bool Channel::isInvited(Client *toCheck){
	std::vector<Client *>::iterator it = std::find(invitedMembers.begin(), invitedMembers.end(), toCheck);
	if (it != invitedMembers.end())
        return true;
    return false;
}
