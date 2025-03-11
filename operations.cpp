#include "server.hpp"
#include "Channel.hpp"

Client* Server::find_user(const std::string &user, Client &client, Channel *channel){
    if (client.get_nick_name() == user)
        puts("cant promote yourself"); // self promotion nono
    else { // khasni n9lb 3lih fchannel
        for (std::map<Client *, bool>::iterator it = channel->getMembers().begin(); it != channel->getMembers().end(); it++){
            if (it->first && it->first->get_nick_name() == user && it->second != true)
                return it->first;
        }
    }
    return NULL;// change this later
}

void Server::process_operation(char sign, const char &oper, Client &client, std::string &arg, Channel *channel){
    if (oper == 'o'){
        if (sign == '+'){
            if (client.get_buffer_size() > 3){
                // Client *newOp = find_user(client.get_cmd(3), i, channel);
                Client *newOp = find_user(arg, client, channel);
                channel->getMembers()[newOp] = true;
                puts("he is now an operator");
            }
        }
        else if (sign == '-'){
            if (client.get_buffer_size() > 3){
                // Client *removeOp = find_user(client.get_cmd(3), i, channel);
                Client *removeOp = find_user(arg, client, channel);
                channel->getMembers()[removeOp] = false;
                puts("7aydna lih operator");
            }
        }
    }
    else if (oper == 'i'){
        if (sign == '+'){
            channel->setInviteOnly(true);// replies ajmi
            send_reply(client.get_socket_fd(), OPER_SUCCESS(client.get_nick_name(), channel->getChannelName(), client.get_ip(), client.get_hostname(), "+i", "MODE"));
        }
        else if (sign == '-'){
            channel->setInviteOnly(false); //replies ajmi
            send_reply(client.get_socket_fd(), OPER_SUCCESS(client.get_nick_name(), channel->getChannelName(), client.get_ip(), client.get_hostname(), "-i", "MODE"));
        }
    }
    else if (oper == 'l'){
        if (sign == '+'){
            if (client.get_buffer_size() > 3){
                // std::string limit = client.get_cmd(3);
                std::string limit = arg;
	            std::stringstream ss(limit);
	            long userLimit = 0;
	            ss >> userLimit;
	            if (!ss.fail() && ss.eof() && (userLimit < 0 || userLimit > 2147483647))
                    puts("error number");
                channel->setLimitSet(true);
                channel->setUserLimit(userLimit);
                std::string loper = "+l " + arg;
                send_reply(client.get_socket_fd(), OPER_SUCCESS(client.get_nick_name(), channel->getChannelName(), client.get_ip(), client.get_hostname(), loper, "MODE"));
            }
        }
        else if (sign == '-'){
            channel->setLimitSet(false);
            send_reply(client.get_socket_fd(), OPER_SUCCESS(client.get_nick_name(), channel->getChannelName(), client.get_ip(), client.get_hostname(), "-l", "MODE"));
        }
    }
    else if (oper == 'k'){
        if (sign == '+'){
            if (client.get_buffer_size() > 3){
                // std::string pass = client.get_cmd(3);
                std::string pass = arg;
                channel->setRequiresPass(true);
                channel->setPass(pass);
                std::string &channelName = channel->getChannelName();
                for (std::vector<std::pair<std::string, std::string> >::iterator it = channelAndPass.begin(); it != channelAndPass.end(); ++it){
                    if (it->first == channelName){
                        it->second = pass;
                    }
                }
            }
            // else
                // Server::send_reply(client.get_socket_fd(), ERR_NEEDMOREPARAMS(client.get_nick_name(), "MODE"));
        }
        else if (sign == '-'){
            channel->setRequiresPass(false);
            channel->setPass(""); // hmmmm tanchof wach blan
        }
    }
    else if (oper == 't'){
        if (sign == '+')
            channel->setTopicFlag(true);
        else if (sign == '-')
            channel->setTopicFlag(false);
    }   
}