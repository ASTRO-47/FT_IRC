#include "server.hpp"
#include "Channel.hpp"

// check channel member limits!!!

// user kay inviti raso hhhhh
void Server::invite_user(const std::string &invited, Client *sender ,const std::string &chan){
    if (channel_exists(chan) && channelMap[chan].getInviteOnly()
        && !channelMap[chan].isOperator(sender)){
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
    // std::vector<std::string> vec = cinvited->getInvitedChannels();
    // std::vector<std::string>::iterator it = std::find(vec.begin(), vec.end(), chan);
    // if (it != vec.end()){
        std::string msg = server_prefix + " you're invited to join the channe\r\n"; //hhhhh
        send_reply(cinvited->get_socket_fd(), msg);
        channelMap[chan].appendInvitedMembers(cinvited);
    // }// ila kan already invited ldik channel n ignori wsf
    // else
        // you werent invited
    // reply mgada

// channel makaynch - user makaynch fdik channel
// invite message
//mat invitich rask
}



// 3awd checki replies d invite dik no such nick


