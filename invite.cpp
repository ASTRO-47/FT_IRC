#include "server.hpp"
#include "Channel.hpp"

void Server::invite_user(const std::string &invited, Client *sender ,const std::string &chan){
    if (channelMap[chan]->getInviteOnly()
        && !channelMap[chan]->isOperator(sender)){
            //ERR_CHANOPRIVSNEEDED
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
        puts("no such nick err");
        return;
    }
    std::map<std::string, Channel *>::iterator iter = channelMap.find(chan);
    if (iter != channelMap.end()){
        std::map<Client *, bool >::iterator member = iter->second->getMembers().find(cinvited);
        if (member != iter->second->getMembers().end()){
            puts("already a member of the channel");
            return;
        }
    }
    // ila kan already fdik channel
    std::string msg = server_prefix + " you're invited to join the channel\n";
    // send_reply(clientsget_socket_fd(), msg);
// channel makaynch - user makaynch fdik channel
// invite message
// mat invitich rask
}