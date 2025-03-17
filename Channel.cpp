#include "Channel.hpp"

Channel::Channel(std::string ChannelName, Client* Creator){
	name = ChannelName;
	members.push_back(std::make_pair(Creator, true));
	isInviteOnly = false;
	pass = "";
	requiresPass = false;
	limitSet = false;
	topicSettable = true;
	topicSet = false;
	topic = "";
	userLimit = -1;
	Server::send_reply(Creator->get_socket_fd(), CHANNEL_JOIN(Creator->get_nick_name(), ChannelName, Creator->get_ip(), Creator->get_user_name()));
	Server::send_reply(Creator->get_socket_fd(), CHANNEL_MODES(Creator->get_nick_name(), ChannelName, Creator->get_ip(), Creator->get_user_name()));
	Server::send_reply(Creator->get_socket_fd(), RPL_NAMREPLY(Creator->get_nick_name(), ChannelName, '@' + Creator->get_nick_name()));
	Server::send_reply(Creator->get_socket_fd(), RPL_ENDOFNAMES(Creator->get_nick_name(), ChannelName));
}

void Channel::appendMember(Client *newMember){
	members.insert(members.begin(),std::make_pair(newMember, false));
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
	members.clear();
}

void Channel::setRequiresPass(const bool &NewStatus){
	requiresPass = NewStatus;
}

bool & Channel::getRequiresPass(){
	return requiresPass;
}

void Channel::removeMember(Client *toRemove){
	std::vector<std::pair<Client*, bool> >::iterator it = members.end();
	for (std::vector<std::pair<Client*, bool> >::iterator it2 = members.begin(); it2 != members.end(); it2++){
		if (it2->first == toRemove){
			it = it2;
			break;
		}
	}
	if (it != members.end()){
		Channel::broadcastToAllMembers(CHANNEL_QUIT(toRemove->get_nick_name(), getChannelName(), toRemove->get_ip(), toRemove->get_user_name()), *this, *toRemove, false);
		members.erase(it);
	}
	std::vector<Client *>::iterator it2 = std::find(invitedMembers.begin(), invitedMembers.end(), toRemove);
	if (it2 != invitedMembers.end())
		invitedMembers.erase(it2);
}

void Channel::kickMember(Client *toRemove){
	std::vector<std::pair<Client*, bool> >::iterator it = members.end();
	for (std::vector<std::pair<Client*, bool> >::iterator it2 = members.begin(); it2 != members.end(); it2++){
		if (it2->first == toRemove){
			it = it2;
			break;
		}
	}
	if (it != members.end()){
		members.erase(it);
	}
	std::vector<Client *>::iterator it2 = std::find(invitedMembers.begin(), invitedMembers.end(), toRemove);
	if (it2 != invitedMembers.end())
		invitedMembers.erase(it2);
}

bool Channel::isMember(Client* client){
	for (std::vector<std::pair<Client*, bool> >::iterator it = members.begin(); it != members.end(); it++){
		if (it->first == client)
			return true;
	}
	return false;
}

bool Channel::isOperator(Client* client){
	for (std::vector<std::pair<Client*, bool> >::iterator it = members.begin(); it != members.end(); it++){
		if (it->first == client)
			return it->second;
	}
	return false;
}

void Channel::setOperator(Client* client, bool flag){
	for (std::vector<std::pair<Client*, bool> >::iterator it = members.begin(); it != members.end(); it++){
		if (it->first == client){
			it->second = flag;
			return;
		}
	}
}

std::vector<std::pair<Client*, bool> >& Channel::getMembers(){
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

void Channel::broadcastToAllMembers(std::string msg, Channel &chan, Client &sender, int selfSend){
	if (selfSend == 1){
		for (std::vector<std::pair<Client*, bool> >::iterator it = chan.members.begin(); it != chan.members.end(); it++){
			if (it->first)
				Server::send_reply(it->first->get_socket_fd(), msg);
		}
	}
	else{
		for (std::vector<std::pair<Client*, bool> >::iterator it = chan.members.begin(); it != chan.members.end(); it++){
			if (it->first && it->first != &sender)
				Server::send_reply(it->first->get_socket_fd(), msg);
		}
	}
}

std::string Channel::print_members(){
	std::stringstream ss;
	for (std::vector<std::pair<Client*, bool> >::iterator it = members.begin(); it != members.end(); it++){
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

void Channel::displayModes(Client &caller){
	std::string buff = "+";
	std::string params;
	if (requiresPass)
		buff += "k";
	if (limitSet)
		buff += "l";
	if (topicSettable)
		buff += "t";
	if (isInviteOnly)
		buff += "i";
	if (buff.find('l') != std::string::npos){
		std::ostringstream oss;
		oss << userLimit;
		if (!oss.fail()){
			params = oss.str();
		}
	}
	Server::send_reply(caller.get_socket_fd(), RPL_CHANNELMODEIS(caller.get_nick_name(), getChannelName(), buff, params));
}
