#include "server.hpp"
#include "Channel.hpp"

// user kay inviti raso hhhhh
void Server::invite_user(const std::string &invited, Client *sender ,const std::string &chan){
    if (channelMap[chan] && channelMap[chan]->getInviteOnly()
        && !channelMap[chan]->isOperator(sender)){
            // send_reply(sender->get_socket_fd(), ERR_CHANOPRIVSNEEDED(sender->get_nick_name(), chan)) knt hna
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
    // ila kan already fdik channel
    std::map<std::string, Channel *>::iterator iter = channelMap.find(chan);
    if (iter != channelMap.end()){
        std::map<Client *, bool >::iterator member = iter->second->getMembers().find(cinvited);
        if (member != iter->second->getMembers().end()){
            std::cout << member->first->get_nick_name() << "-+-+" << '\n';
            puts("already a member of the channel");
            return;
        }
    }
    // std::vector<std::string> vec = cinvited->getInvitedChannels();
    // std::vector<std::string>::iterator it = std::find(vec.begin(), vec.end(), chan);
    // if (it != vec.end()){
        std::string msg = server_prefix + " you're invited to join the channe\r\n";
        send_reply(cinvited->get_socket_fd(), msg);
        cinvited->appendInvitedChannels(chan);
    // }// ila kan already invited ldik channel n ignori wsf
    // else
        // you werent invited
    // reply mgada

// channel makaynch - user makaynch fdik channel
// invite message
// mat invitich rask
}
