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
    if (kicked.empty() || chan.empty())
        return false; // appropriate error
    if (oper->get_nick_name() == kicked){
        puts("can't kick yourself");
        return false;
    }
    Client *toKick = NULL;
    toKick = find_client(kicked);
    if (!toKick){
        puts("client makaynch");
        return false;
    }
    // segv here when user changes his nick after logging in
    if (!channelMap[chan.substr(1)]->isMember(oper) || !channelMap[chan.substr(1)]->isOperator(oper) || !channelMap[chan.substr(1)]->isMember(toKick)){
        puts("machi operator || machi member dak lighaytkicka");
        return false;
    }
    channelMap[chan.substr(1)]->removeMember(toKick);
    puts("kicked");
    return true;
}

void Server::kick_handler(Client &client, size_t buffer_size){
	    if (buffer_size < 3){
        // need more params
        return;
    }
    else if (buffer_size == 3){
        std::string chan = client.get_cmd(1);
        std::string kicked = client.get_cmd(2);
        kick_user(&client, kicked, chan);
        // print_reason ansawbha wnwli ncalliha
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