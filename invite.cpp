#include "server.hpp"
#include "Channel.hpp"

void Server::invite_user(const std::string &invited, Client *sender ,const std::string &chan){
    if (channel_exists(chan) && channelMap[chan].getInviteOnly()
        && !channelMap[chan].isOperator(sender)){
            send_reply(sender->get_socket_fd(), ERR_CHANOPRIVSNEEDED(sender->get_nick_name(), chan));
            return;
    }
    Client *cinvited = NULL;
    for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); it++){
        if ((*it)->get_nick_name() == invited){
            cinvited = *it;
            break;
        }
    }
    if (!cinvited){
        send_reply(sender->get_socket_fd(), ERR_NOSUCHNICK(sender->get_nick_name(), invited));
        return;
    }
    std::map<std::string, Channel>::iterator iter = channelMap.find(chan);
    if (iter != channelMap.end()){
	    for (std::vector<std::pair<Client*, bool> >::iterator it = iter->second.getMembers().begin(); it != iter->second.getMembers().end(); it++){
            if (it->first == cinvited){
                send_reply(sender->get_socket_fd(), ERR_USERONCHANNEL(invited, chan, sender->get_nick_name()));
                return;
            }
        }
    }
    send_reply(cinvited->get_socket_fd(), RPL_INVITE(sender->get_nick_name(), chan, sender->get_ip(), sender->get_hostname(), invited));
    send_reply(sender->get_socket_fd(), RPL_INVITING(sender->get_nick_name(), invited, chan));
    channelMap[chan].appendInvitedMembers(cinvited);
}