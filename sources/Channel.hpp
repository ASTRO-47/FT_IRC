#pragma once

#include "server.hpp"
class Client;

class Channel{
	private:
	std::string					name;
	std::string					pass;
	std::string					topic;
	std::vector<std::pair<Client*, bool> > members;
	bool 						isInviteOnly;
	bool 						requiresPass;
	bool 						limitSet;
	size_t 						userLimit;
	bool 						topicSettable;
	bool 						topicSet;
	std::vector<Client*> 		invitedMembers;
	public:
	Channel();
	std::string & 				getTopicString();
	std::string & 				getPass();
	bool & 						getRequiresPass();
	bool 						isOperator(Client*);
	bool 						isMember(Client*);
	void 						setOperator(Client* ,bool);
	
	void 						setPass(const std::string&);
	void 						setTopic(const std::string&);
	void 						setRequiresPass(const bool&);
	void 						appendMember(Client *);
	void 						setInviteOnly(bool);
	bool & 						getInviteOnly();
	std::vector<std::pair<Client*, bool> >& getMembers();
	Channel(std::string, Client*);
	~Channel();
	void 						setUserLimit(size_t newLimit);
	size_t & 					getUserLimit();
	bool & 						getLimitSet();
	void 						setLimitSet(bool flag);
	unsigned long 				getNumMembers();
	bool & 						getTopicFlag();
	bool & 						getTopic();
	void 						setTopic(bool);
	void 						setTopicFlag(bool);
	std::string & 				getChannelName();
	void 						removeMember(Client *);
	void 						kickMember(Client *);
	static void 				broadcastToAllMembers(std::string, Channel &, Client &, bool);
	std::string 				print_members();
	bool 						isInvited(Client *);
	std::vector<Client*>& 		getInvitedMembers();
	void 						appendInvitedMembers(Client *);
	void 						setTopicString(std::string &);
	void 						displayModes(Client &);
};