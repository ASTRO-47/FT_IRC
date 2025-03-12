#include "server.hpp"
#include "Channel.hpp"

Client* Server::find_client(std::string &tokick){
    for (size_t i = 0; i < clients.size(); i++){
        if (clients[i]->get_nick_name() == tokick)
            return clients[i];
    }
    return NULL;
}

// maykikich raso
// checki wach khona mod
// wach kicked valid user 
bool Server::kick_user(Client *oper, std::string &kicked, std::string &chan){
    if (kicked.empty() || chan.empty()){
        send_reply(oper->get_socket_fd(), ERR_NEEDMOREPARAMS(oper->get_nick_name(), "KICK"));
        return false;
    }
    if (oper->get_nick_name() == kicked){ // labgha ykicki raso kayna part
        send_reply(oper->get_socket_fd(), ERR_NEEDMOREPARAMS(oper->get_nick_name(), "KICK"));
        return false;
    }
    Client *toKick = NULL;
    toKick = find_client(kicked);
    if (!toKick){
        send_reply(oper->get_socket_fd(), ERR_NOSUCHNICK(oper->get_nick_name(), kicked));
        return false;
    }
    // segv here when user changes his nick after logging in
    if (!channelMap[chan]->isMember(oper)){
        send_reply(oper->get_socket_fd(), ERR_NOTONCHANNEL(chan, oper->get_nick_name()));
        return false;
    }
     else if (!channelMap[chan]->isOperator(oper)){
        send_reply(oper->get_socket_fd(), ERR_CHANOPRIVSNEEDED(oper->get_nick_name(), chan));
        return false;
     }
     else if (!channelMap[chan]->isMember(toKick)){
        send_reply(oper->get_socket_fd(), ERR_USERNOTINCHANNEL(chan, oper->get_nick_name(), kicked));
        return false;
     }
    channelMap[chan]->removeMember(toKick);
    std::string msg = kicked + " :" + oper->get_nick_name();
    channelMap[chan]->broadcastToAllMembers(OPER_SUCCESS(oper->get_nick_name(), channelMap[chan]->getChannelName(), oper->get_ip(), oper->get_hostname(), msg, "KICK"));
    return true;
}

void Server::kick_handler(Client &client, size_t buffer_size){
	    if (buffer_size < 3){
        send_reply(client.get_socket_fd(), ERR_NEEDMOREPARAMS(client.get_nick_name(), "KICK"));
        return;
    }
    else if (buffer_size == 3){
        std::string chan = client.get_cmd(1);
        std::string kicked = client.get_cmd(2);
        kick_user(&client, kicked, chan);
    }
    else if (buffer_size > 3){
        std::vector<std::string> vec = client.get_cmd_buffer();
        std::vector<std::string>::iterator it = std::find(vec.begin(), vec.end(), ":");
        if (it != vec.end()){
            it++;
            std::string reason;
            while (it != vec.end()){
                reason += *it + ' ';
                it++;
            }
            size_t pos = client.get_buffer().find(':');
            if (pos != std::string::npos){
                std::string reason = client.get_buffer().substr(pos + 1, client.get_buffer().length() - pos - 2);
            }
            else
                std::string reason = client.get_cmd(2);
        }
    }
}

// lcase dial no such nick 3ad lcase dial ila kan wlkn makaynch f channel


// ana makaynch fdik channel w baghi nkicki chi7d jayn fdik channel